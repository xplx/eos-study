/**
 * 用于内联操作
 */
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract]] addressbook : public eosio::contract {

public:
  using contract::contract;

  addressbook(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  [[eosio::action]]
  void upsert(name user, std::string first_name, std::string last_name, uint64_t age, std::string street, std::string city, std::string state) {
    require_auth( user );
    address_index addresses(_code, _code.value);
    auto iterator = addresses.find(user.value);
    if( iterator == addresses.end() )
    {
      addresses.emplace(user, [&]( auto& row ) {
       row.key = user;
       row.first_name = first_name;
       row.last_name = last_name;
       row.age = age;
       row.street = street;
       row.city = city;
       row.state = state;
      });
      //内联操作
      send_summary(user, " successfully emplaced record to addressbook");
    }
    else {
      std::string changes;
      addresses.modify(iterator, user, [&]( auto& row ) {
        row.key = user;
        row.first_name = first_name;
        row.last_name = last_name;
        row.street = street;
        row.city = city;
        row.state = state;
        row.age = age;
      });
      send_summary(user, " successfully modified record to addressbook");
    }
  }

  [[eosio::action]]
  void erase(name user) {
    require_auth(user);
    address_index addresses(_self, _code.value);
    auto iterator = addresses.find(user.value);
    eosio_assert(iterator != addresses.end(), "Record does not exist");
    addresses.erase(iterator);
    send_summary(user, " successfully erased record from addressbook");
  }

  /**
   * 此事务需要复制到用户，因此可以将其视为收据。
   */
  [[eosio::action]]
    void notify(name user, std::string msg) {
	  //要求调用此操作的权限来自合同,验证用户name，防止“伪造”此合同中的收据。
      require_auth(get_self());
      //添加收据通知
      require_recipient(user);
    }

private:
  struct [[eosio::table]] person {
    name key;
    std::string first_name;
    std::string last_name;
    uint64_t age;
    std::string street;
    std::string city;
    std::string state;

    uint64_t primary_key() const { return key.value; }
    uint64_t get_secondary_1() const { return age;}

  };

  void send_summary(name user, std::string message) {
      action(
        permission_level{get_self(),"active"_n}, //验证权限
        get_self(), //调用的代码
        "notify"_n, //执行的动作
        std::make_tuple(user, name{user}.to_string() + message)
      ).send();
    };

  typedef eosio::multi_index<"people"_n, person, indexed_by<"byage"_n, const_mem_fun<person, uint64_t, &person::get_secondary_1>>> address_index;

};

EOSIO_DISPATCH( addressbook, (upsert)(notify)(erase))

/**
 * cleos get table addressbook addressbook people --upper 10 --key-type i64 --index 2 （其中的upper小于10，lower大于10）
 */
