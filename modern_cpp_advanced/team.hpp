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
#include <iostream>
#include <string>
#include <vector>  // includes <initializer_list>
#include <cassert>

template<typename T = std::string>
class Team {
  std::vector<std::string> names_;
  std::vector<T> values_;
 public:
  Team() = delete;
  Team(const std::initializer_list<std::string>& names) 
   : names_(names) {
      //std::cout << "size: " << names.size() << '\n';
      assert(names.size() > 1 ||
             (names.size() == 1 && !names.begin()->empty()));
  }

  // for list of ints within {...} :
  void insert(const std::initializer_list<T>& values) {
    values_.insert(values_.end(), values);
  }
  // OPTION 1: recursively:
  /*
  void insert() {
  }
  template<typename T, typename... Args>
  void insert(T firstArg, Args... values) {
    values_.push_back(firstArg);
    insert(values...);
  }
  */
  // OPTION 2: no compile-time recursion:
  template<typename... Args>
  void insert(Args... values) {
    values_.insert(values_.end(), {static_cast<T>(values)...});
  }
  void insert() = delete;

  // getter for the values:
  const std::vector<T>& getValues() const {
    return values_;
  }

  friend std::ostream& operator<<(std::ostream& os, const Team& team) {
    os << "[ ";
    for (const auto& name : team.names_) {
      os << name << ' ';
    }
    os << ": ";
    for (const auto& value : team.values_) {
      os << value << ' ';
    }
    os << ']';
    return os;
  }

  /*
  // print when a team is copied / copy-assigned:
  // -implement copy constructor/assignment:
  Team(const Team& other)
   : names_{other.names_}, values_{other.values_} {
      std::cout << "COPY " << *this << '\n'; 
  }
  Team& operator=(const Team& other) {
    std::cout << "COPYASSIGN " << other << '\n'; 
    values_ = other.values_;
    names_ = other.names_;
    return *this;
  }
  // move-support:
  Team(Team&& other) = default;  // generates noexcept guarantees for you
  Team(Team&& other) noexcept(std::is_nothrow_move_constructible<decltype(values_)>::value &&
                              std::is_nothrow_move_constructible<decltype(names_)>::value)
   : names_{std::move(other.names_)}, values_{std::move(other.values_)} {
      std::cout << "MOVE " << *this << '\n'; 
  }
  Team& operator=(Team&& other) {
    std::cout << "MOVEASSIGN " << other << '\n'; 
    values_ = std::move(other.values_);
    names_ = std::move(other.names_);
    return *this;
  }
  */
};


