#include <thread>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <condition_variable>

std::atomic<bool> chopping(true);
std::mutex chop_mutex;
unsigned int shared_count = 0;
void vegetable_chopper(const char* name) {
    unsigned int personal_count = 0;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(chop_mutex);
            if (!chopping) {
                chopping = false;
                break;
            }
            shared_count++;
            personal_count++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << name << " chopped " << personal_count << " vegetables in total.\n";
}

std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> helloDone(false);
void say_hello() {
    while(true) {
        std::cout << "Hello" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        helloDone = true; // Actualizamos directamente el valor atómico
        cv.notify_one();  // Notificamos al otro hilo
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void say_goodbye() {
    while(true) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return helloDone.load(); }); // Esperamos a que helloDone sea true
        }
        std::cout << "Goodbye" << std::endl;
        helloDone = false; // Actualizamos el valor atómico
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    /**/
    // Chopping vegetables
    std::thread olivia(vegetable_chopper, "Olivia");
    std::thread barron(vegetable_chopper, "Barron");
    printf("Barron and Olivia are chopping vegetables.. \n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    chopping = false;
    barron.join();
    olivia.join();
    std::cout << "Total vegetables chopped: " << shared_count << std::endl;
    /**/

    /*
    // Saying Hello and GoodBye
    std::thread hello_thread(say_hello);
    std::thread goodbye_thread(say_goodbye);

    hello_thread.join();
    goodbye_thread.join();
    */
}