// Let's initialize a class Team having
// - multiple names
// - multiple floating-point values
// and an API to
// - create a team with one or more names
// - insert one or more values
// - print out a team (names and values)
// eg.
// - a Team "TeamA" has members "Jim" and "Gianna"
//   and the values 0, 8, 15, 7.7, -32

#include "team.hpp"
#include <vector>
#include <deque>
#include <array>
#include <list>
#include <forward_list>
#include <unordered_map>
#include "buckets.hpp"  // for printHashTableState()
#include <algorithm>    // for std::count_if()
#include <numeric>      // for std::accumulate()

// helper function to print the first two and the last element
// of a collection and dots in between (if the are more than 3 elements):
// e.g.:
//  vector<int> coll{1, 2, 3, 4, 5}
// printColl(coll) prints:
//   1
//   2
//   ...
//   5
template<typename T>
void printColl(const T& coll)
{
  std::cout << "coll:\n";
  int idx = 0;
  for (auto pos = coll.begin(); pos != coll.end() && idx < 2; ++pos) {
    std::cout << ' ' << *pos << '\n';
    ++idx;
  }
  if (coll.size() >= 4) { // dots if more than 3:
    std::cout << " ...\n";
  }
  if (coll.size() > 2) {
    std::cout << ' ' << coll.back() << '\n';
  }
}

void testPrintColl()
{
  std::cout << "==== testPrintColl()\n";
  printColl(std::vector<int>{1, 2, 3, 4, 5});  // 1 2 ... 5
  printColl(std::vector<int>{1, 2, 3});        // 1 2 3
  printColl(std::vector<int>{1, 2});           // 1 2
  printColl(std::vector<int>{1});              // 1
  printColl(std::vector<int>{});               // 
  printColl(std::vector<std::string>{"Rome", "Braunschweig", "Berlin", "Turino"}); 
  printColl(std::array<int,8>{1, 2, 3});               // 
  printColl(std::deque<int>{1,2,3,4,5});               // 
  printColl(std::list<int>{1,2,3,4,5});               // 
  //printColl(std::forward_list<int>{1,2,3,4,5});               // 
}

// count how many teams have a sum of values > maxSum:
int countMoreThan(const std::unordered_map<std::string, Team<double>>& teams,
                  int maxSum)
{
  return std::count_if(teams.begin(), teams.end(),
                       [maxSum, &teams](const auto& team) {
                         /*
                         // if getValues() returns by value:
                         auto values = team.second.getValues();
                         return std::accumulate(values.begin(), values.end(),
                                                0.0) > maxSum;
                         */
                         // if getValues retrurns by reference:
                         return std::accumulate(team.second.getValues().begin(),
                                                team.second.getValues().end(),
                                                0.0) > maxSum;
                       });
}

template <typename T>
void printAllTeams(const T& teams)
{
  std::cout << "All teams: \n";
  for (const auto& team : teams) {
    std::cout << "  " << team.first << ": " << team.second << '\n';
  }
}

void testAssoContainer()
{
  std::cout << "==== testAssoContainer()\n";
  // associative container for teams
  // - mapping a team name to a team
  // - create and iterate over them to print them out
  // - try map or unordered_map
  Team<double> teamA{"Jim", "Gianna", "Andrea"};
  teamA.insert(42, 7, 5.5);
  Team<double> teamB{"Tom", "Maria", "Guido"};
  teamB.insert(0, 0, 0);
  Team<double> teamC{"Nicola"};
  teamC.insert(42);

  // insert teams into a map:
  std::cout << "---- insert into map:\n";
  std::unordered_map<std::string, Team<double>> teams {
    {"Team A", teamA},
    {"Team B", teamB}
  };
  teams.max_load_factor(0.7f);
  std::cout << "---- \n";
  teams.insert({"Team C", teamC});

  std::cout << "---- print all teams:\n";
  printAllTeams(teams);
  std::cout << "sum > 20:  " << countMoreThan(teams, 20) << '\n'; 
  std::cout << "sum > 100: " << countMoreThan(teams, 100) << '\n'; 

  Team<double> team100{"Jim", "Tina"};
  team100.insert(50, 50.1);
  teams.insert({"Team 100", team100});
  printAllTeams(teams);
  std::cout << "sum > 100: " << countMoreThan(teams, 100) << '\n'; 

  //printHashTableState(teams);
  std::hash<std::string> hf;
  std::cout << "hash value of Team A:   " << hf("Team A") << '\n'; 
  std::cout << "hash value of Team B:   " << hf("Team B") << '\n'; 
  std::cout << "hash value of Team C:   " << hf("Team C") << '\n'; 
  std::cout << "hash value of Team 100: " << hf("Team 100") << '\n'; 

  // use an unordered container with my own hash function:
  struct StringHasher {
    std::size_t operator() (const std::string& s) const {
      return s.size();   // VERY VERY VERY BAD hash function
    }
  };
  std::unordered_map<std::string, Team<double>, StringHasher> teams2;
  teams2.insert({"Team A", std::move(teamA)});
  teams2.insert({"Team B", std::move(teamB)});
  teams2.insert({"Team C", std::move(teamC)});
  teams2.insert({"Team 100", std::move(team100)});
  printAllTeams(teams2);
  //printHashTableState(teams2);

  for (int i=0; i<20; ++i) {
    Team<double> t{"unknown"};
    t.insert(i, i*100, i*1000, -i);
    //teams.insert(std::pair<std::string,Team<double>>{"Team " + std::to_string(i), std::move(t)});
    //teams.insert({"Team " + std::to_string(i), std::move(t)});
    teams.emplace("Team " + std::to_string(i), std::move(t));
    // does not work because we have no default constructor for Team:
    //teams["Team " + std::to_string(i)] = t;
  }
  printAllTeams(teams);
  //printHashTableState(teams);

  // count how many teams have a sum of values > 100:
  auto num = countMoreThan(teams, 100);
  std::cout << "sum > 100: " << num << '\n'; 
}

void testTeam()
{
  std::cout << "==== testTeam()\n";
  Team<double> teamA{"Jim", "Gianna", "Andrea"};
  //teamA.insert({42, 7, 5.5});
  teamA.insert(42, 7, 5.5);
  teamA.insert(42.7);
  //teamA.insert();   // deleted as special case
  std::cout << teamA << '\n';
  //Team teamB;   // NO, we have no default constructor
  //Team teamC{}; // NO: default constructor is deleted
  //Team teamD(); // NO: function declaration !
  //Team teamE({}); // assertion fails
  //std::cout << teamE << '\n';

  // Team with string values (default value type):
  Team<> teamS{"Jim", "Gianna", "Andrea"};
  teamS.insert("42", "7", "5.5");
  teamS.insert("another interesting value");
  std::cout << teamS << '\n';
}

int main()
{
  testTeam();
  //testPrintColl();
  testAssoContainer();
}
