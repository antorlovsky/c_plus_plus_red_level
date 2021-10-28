#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <cstdint>
#include <queue>

using namespace std;

struct Booking {
    int64_t time;
    string hotel;
    int client_id;
    int64_t room_count;
};

class HotelManager {
public:
    HotelManager() {}

    void Book(int64_t time, const string& hotel, int client_id, int64_t room_count) {
        ReduceTime(time);

        bookings.push({time, hotel, client_id, room_count});
        clients[hotel].insert(client_id);

        if (rooms.count(hotel) == 1) {
            clients[hotel].insert(client_id);
            rooms[hotel] += room_count;
        }
        else {
            clients[hotel] = {client_id};
            rooms[hotel] = room_count;
        }
    }

    uint64_t GetClients(const string& hotel) const {
        return clients.count(hotel) == 1 ? clients.at(hotel).size() : 0;
    }

    uint64_t GetRooms(const string& hotel) const {
        return rooms.count(hotel) == 1 ? rooms.at(hotel) : 0;
    }

private:
    static const int SECONDS_IN_DAY = 86'400;

    map<string, set<int>> clients;
    map<string, int64_t> rooms;
    queue<Booking> bookings;

    void ReduceTime(const int64_t& time) {
        while (!bookings.empty() && bookings.front().time <= (time - SECONDS_IN_DAY)) {
            auto booking = bookings.front();
            bookings.pop();

            clients[booking.hotel].erase(booking.client_id);
            rooms[booking.hotel] -= booking.room_count;
        }
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    HotelManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;

        if (query_type == "BOOK") {
            int64_t time;
            string hotel;
            int client_id;
            int64_t room_count;
            cin >> time >> hotel >> client_id >> room_count;

            manager.Book(time, hotel, client_id, room_count);
        }
        else if (query_type == "CLIENTS") {
            string hotel;
            cin >> hotel;

            cout << manager.GetClients(hotel) << "\n";
        }
        else if (query_type == "ROOMS") {
            string hotel;
            cin >> hotel;

            cout << manager.GetRooms(hotel) << "\n";
        }
    }

    return 0;
}