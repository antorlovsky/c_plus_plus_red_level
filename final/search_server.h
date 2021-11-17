#pragma once

#include "sinchronized.h"

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <future>

using namespace std;

class InvertedIndex {
public:
    struct DocHits {
        size_t docid;
        size_t hit_count;
    };

    InvertedIndex() = default;

    explicit InvertedIndex(istream& document_input);

    const vector<InvertedIndex::DocHits>& Lookup(string_view word) const;

    const string& GetDocument(size_t id) const {
        return docs[id];
    }

    const size_t GetDocumentCount() const {
        return docs.size();
    }

private:
    map<string_view, vector<DocHits>> index;
    deque<string> docs;
};

class SearchServer {
public:
    SearchServer() = default;

    explicit SearchServer(istream& document_input) : index(InvertedIndex(document_input)) {}

    void UpdateDocumentBase(istream& document_input);

    void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
    Synchronized<InvertedIndex> index;
    vector<future<void>> futures;
};
