#include "search_server.h"
#include "iterator_range.h"
#include "parse.h"

#include <algorithm>
#include <numeric>
#include <iterator>
#include <sstream>
#include <iostream>

InvertedIndex::InvertedIndex(istream& document_input) {
    for (string document; getline(document_input, document);) {
        docs.push_back(move(document));

        const size_t docid = docs.size() - 1;

        for (string_view word: SplitIntoWords(docs.back())) {
            auto& doc_hit = index[word];

            if (doc_hit.empty() || doc_hit.back().docid != docid)
                doc_hit.push_back({docid, 1});
            else
                doc_hit.back().hit_count++;
        }
    }
}

const vector<InvertedIndex::DocHits>& InvertedIndex::Lookup(string_view word) const {
    static const vector<DocHits> empty_result;

    auto it = index.find(word);

    if (it != index.end())
        return it->second;
    else
        return empty_result;
}

void UpdateDocumentBaseAsync(istream& document_input, Synchronized<InvertedIndex>& index) {
    InvertedIndex new_index(document_input);
    swap(index.GetAccess().ref_to_value, new_index);
}

void AddQueriesStreamAsync(istream& query_input, ostream& search_results_output, Synchronized<InvertedIndex>& index) {
    vector<size_t> docid_count;
    vector<int64_t> docid_count_idx;

    for (string current_query; getline(query_input, current_query);) {
        {
            auto access = index.GetAccess();
            size_t doc_count = access.ref_to_value.GetDocumentCount();
            docid_count.assign(doc_count, 0);

            for (string_view word: SplitIntoWords(current_query)) {
                for (const auto& [docid, hit_count] : access.ref_to_value.Lookup(word)) {
                    docid_count[docid] += hit_count;
                }
            }
        }

        docid_count_idx.resize(docid_count.size());
        iota(docid_count_idx.begin(), docid_count_idx.end(), 0);

        partial_sort(
                docid_count_idx.begin(),
                Head(docid_count_idx, 5).end(),
                docid_count_idx.end(),
                [&docid_count](int64_t lhs, int64_t rhs) {
                    return make_pair(docid_count[lhs], -lhs) > make_pair(docid_count[rhs], -rhs);
                }
        );

        search_results_output << current_query << ':';
        for (auto docid : Head(docid_count_idx, 5)) {
            size_t hit_count = docid_count[docid];

            if (hit_count == 0)
                break;

            search_results_output << " {"
                                  << "docid: " << docid << ", "
                                  << "hitcount: " << hit_count << '}';
        }

        search_results_output << endl;
    }
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    futures.push_back(async(UpdateDocumentBaseAsync, ref(document_input), ref(index)));
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
    futures.push_back(async(AddQueriesStreamAsync, ref(query_input), ref(search_results_output), ref(index)));
}
