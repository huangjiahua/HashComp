#include <iostream>
#include <string>
#include <iomanip>
#include "helper.h"
#include <limits>
#include <locale>

using namespace std;

struct my_numpunct : numpunct<char> {
    string do_grouping() const { return "\3"; }
};

int main(int argc, const char *argv[]) {
    string ht_name = "ebr";
    size_t thd_cnt = 4;
    size_t task_num = 1000000;
    size_t range = numeric_limits<size_t>::max();
    size_t data_cnt = 100000;
    if (argc > 1)
        ht_name = string(argv[1]);
    if (argc > 2) {
        thd_cnt = stoull(string(argv[2]));
        if (thd_cnt > 128) {
            cerr << "Too many threads" << endl;
            exit(1);
        }
    }
    if (argc > 3)
        task_num = stoull(string(argv[3]));
    if (argc > 4) {
        data_cnt = stoull(string(argv[4]));
    }
    size_t ud_task = data_cnt * 2;
    vector<uint64_t> data;
    generate_data(data, data_cnt, uniform_int_distribution<uint64_t>(0, range));
    vector<size_t> time(4, 0);
    vector<thread> threads(thd_cnt);
    time_vec = std::vector<size_t>(thd_cnt, 0ull);

    cout << "TASK BEGINS:  " << ht_name << endl;
    cout << "THREAD COUNT: " << thd_cnt << endl;
    cout << "INSERT " << setw(9) << data_cnt << " UINT_64" << endl;
    cout << "GET    " << setw(9) << task_num << " UINT_64" << endl;
    cout << "UPDATE " << setw(9) << ud_task << " UINT_64" << endl;
    cout << "REMOVE " << setw(9) << data_cnt << " UINT_64" << endl;
    cout << "-----------------------------\n" << endl;
    if (ht_name == "unsafe") {
        unsafe ht;
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(insert_func<unsafe>, &ht, ref(data), thd_cnt, i);
        }
        for (auto &t : threads) t.join();
        time[0] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(get_func<unsafe>, &ht, ref(data), thd_cnt, i, task_num);
        }
        for (auto &t : threads) t.join();
        time[1] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(update_func<unsafe>, &ht, ref(data), thd_cnt, i, ud_task);
        }
        for (auto &t : threads) t.join();
        time[2] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(remove_func<unsafe>, &ht, ref(data), thd_cnt, i);
        }
        for (auto &t : threads) t.join();
        time[3] = average_time();
    } else if (ht_name == "mhash") {
        mhash ht;
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(insert_func<mhash>, &ht, ref(data), thd_cnt, i);
        }
        for (auto &t : threads) t.join();
        time[0] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(get_func<mhash>, &ht, ref(data), thd_cnt, i, task_num);
        }
        for (auto &t : threads) t.join();
        time[1] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(update_func<mhash>, &ht, ref(data), thd_cnt, i, ud_task);
        }
        for (auto &t : threads) t.join();
        time[2] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(remove_func<mhash>, &ht, ref(data), thd_cnt, i);
        }
        for (auto &t : threads) t.join();
        time[3] = average_time();
    } else if (ht_name == "ebr") {
        ebr ht(thd_cnt);
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(insert_func<ebr>, &ht, ref(data), thd_cnt, i);
        }
        for (auto &t : threads) t.join();
        time[0] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(get_func<ebr>, &ht, ref(data), thd_cnt, i, task_num);
        }
        for (auto &t : threads) t.join();
        time[1] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(update_func<ebr>, &ht, ref(data), thd_cnt, i, ud_task);
        }
        for (auto &t : threads) t.join();
        time[2] = average_time();
        for (size_t i = 0; i < thd_cnt; i++) {
            threads[i] = thread(remove_func<ebr>, &ht, ref(data), thd_cnt, i);
        }
        for (auto &t : threads) t.join();
        time[2] = average_time();
    }

    vector<double> tp(4, 0);
    for (int i = 0; i < 4; i++) {
        size_t op = (i == 0 || i == 3) ? data_cnt : task_num;
        op = (i == 2) ? ud_task : op;
        tp[i] = (double) op / (double) time[i] * 1000000;
    }

    locale loc(cout.getloc(), new my_numpunct);
    cout.imbue(loc);
    cout << fixed << setprecision(0);
    cout << "INSERT TIME:  " << setw(7) << (double) time[0] / 1000.0 << " ms"
         << "  Throughput: " << setw(16) << tp[0] << " opt/s" << endl;
    cout << "GET    TIME:  " << setw(7) << (double) time[1] / 1000.0 << " ms"
         << "  Throughput: " << setw(16) << tp[1] << " opt/s" << endl;
    cout << "UPDATE TIME:  " << setw(7) << (double) time[2] / 1000.0 << " ms"
         << "  Throughput: " << setw(16) << tp[2] << " opt/s" << endl;
    cout << "REMOVE TIME:  " << setw(7) << (double) time[3] / 1000.0 << " ms"
         << "  Throughput: " << setw(16) << tp[3] << " opt/s" << endl;
    return 0;
}