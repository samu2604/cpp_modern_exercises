#include "team.hpp"
#include <algorithm>
#include <chrono>
#include <future>
#include <numeric>
#include <mutex>

Team<double> createTeamDouble()
{
  static int id=0;

  Team<double> t{"name"+std::to_string(id)};
  t.insert(-id);
  ++id;
  return t;
}

// insert a team twice into the container:
// - with perfect support of move semantics
template<typename Cont, typename TeamT>
void insertTwice(Cont& cont, TeamT&& team)
{
  cont.push_back(team);                       // copy
  //NO!!!: cont.push_back(std::move(team));  // move always
  cont.push_back(std::forward<TeamT>(team));  // move, if passed with move
}

// ensure the output of line-by-line is synchronized
// - so that we mix lines
// - but never mix charcters inside lines
template<typename T>
void printAllTeams(std::mutex& coutMutex, const T& coll) {
  for (const auto& elem: coll) {
    std::lock_guard<std::mutex> lg{coutMutex};  // get exclusive access to cout
    std::cout << "  " << elem << '\n';
  }
}

int main()
{
  std::vector<Team<double>> coll;
  //coll.reserve(1000);
  for (int i=0; i<100; ++i) {
    coll.push_back(createTeamDouble());
    //coll.emplace_back(createTeamDouble());
    std::cout << "capacity: " << coll.capacity() << '\n'; 
  }
  coll[4].insert(42);
  Team<double> teamX{"some name for 2 teams"};
  insertTwice(coll, teamX);
  std::cout << "teamX: " << teamX << '\n';
  insertTwice(coll, std::move(teamX));
  std::cout << "teamX: " << teamX << '\n';
  //...
  // allow to insert an arbitrary number of temas with and without move semantics:
  //insertTwice(coll, teamA, teamB, std::move(teamC));

  // print all teams and compute the sum of all values in parallel:
  // - force a new thread (don't defer the calls)
  std::mutex coutMutex;
  std::future<void> print1 = std::async(std::launch::async,
                                        printAllTeams<decltype(coll)>,
                                        std::ref(coutMutex), std::cref(coll));
  std::future<void> print2 = std::async(std::launch::async,
                                        printAllTeams<decltype(coll)>,
                                        std::ref(coutMutex), std::cref(coll));
  std::future<double> processSum = std::async(std::launch::async,
                                          [&coll] {
                                            // make it easy with simple loops:
                                            double totalSum = 0;
                                            for (const auto& team : coll) {
                                              for (const auto& value : team.getValues()) {
                                                totalSum += value;
                                              }
                                            }
                                            // not synchonized with the other outputs
                                            // because it doesn't use the mutex:
                                            std::cout << "processSum() done\n";
                                            return totalSum;
                                            /* Option B:
                                            return std::accumulate(coll.begin(), coll.end(),
                                                           0.0,  // initial value
                                                           [](double val, const auto& team) {
                                                             const auto& values = team.getValues();
                                                             return std::accumulate(values.begin(),
                                                                                    values.end(),
                                                                                    val);
                                                           });
                                            */
                                          });
  print1.get();
  print2.get();
  std::cout << "total sum: " << processSum.get() << '\n';

  std::cout << "---- sort:\n";
  // measure how long the sorting takes:
  auto start = std::chrono::steady_clock::now();
  std::sort(coll.begin(), coll.end(),
            [] (const auto& t1, const auto& t2) {
              return t1.getValues().size() < t2.getValues().size();
            });
  auto end = std::chrono::steady_clock::now();
  auto diff = end - start;

  // using the unit of the system clock:
  std::cout << "sort() takes " << diff.count() << '\n';
  std::cout << "unit type is: " << decltype(diff)::period::num << '/' 
                                << decltype(diff)::period::den << "s\n";
  // using milliseconds as double:
  std::chrono::duration<double, std::milli> diffms = end - start;
  std::cout << "sort() takes " << diffms.count() << "ms\n";
  // casting to milliseconds:
  std::cout << "sort() takes "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count()
            << "ns\n";

  //for (const auto& elem: coll) {
  //  std::cout << "  " << elem << '\n';
  //}
}
