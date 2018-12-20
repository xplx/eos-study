/**
 * 实现数据库增删改查
 * sudo eosio-cpp -o addressbook.wasm addressbook.cpp -abigen
 * cleos set contract addressbook /home/wxp/contracts/addressbook
 * cleos push action addressbook myaction '["alice"]' -p alice
 *
 * cleos get table addressbook addressbook address
 *
 *
 */
#include <eosiolib/eosio.hpp>
using namespace eosio;
using namespace std;
class [[eosio::contract]]addressbook: contract {
private:
	struct [[eosio::table]] address {
		name account_name;
		string first_name;
		string last_name;
		string street;
		string city;
		string state;
		uint32_t zip = 0;
		uint64_t primary_key() const {
			return account_name.value;
		}
		uint64_t by_zip() const {
			return zip;
		}
	};
public:
	addressbook(name receiver, name code, datastream<const char*> ds) :
			contract(receiver, code, ds) {
	}
	typedef eosio::multi_index<name("address"), address, indexed_by < name("zip"),
			const_mem_fun<address, uint64_t, &address::by_zip>>> address_index;

	[[eosio::action]]
	void myaction(name user, string city, uint32_t zip) {
		//验证
		require_auth(user);
		address_index addresses(_self, _self.value); // code, scope
		// add to table, first argument is account to bill for storage
		//返回end时，表示没有找到相应的对象
		auto itr = addresses.find(user.value);
		if (itr == addresses.end()) {
			addresses.emplace(user, [&](auto& address) {
				address.account_name = user;
				address.first_name = "Daniel";
				address.last_name = "Larimer";
				address.street = "1 EOS Way";
				address.city = "Blacksburg";
				address.state = "VA";
				address.zip = zip;
			});
		} else {
			//_self:付款人是一个有效执行操作的有效帐户，需要为存储使用付费
			eosio_assert(itr != addresses.end(), "Lock arf, Address for account not found");
			addresses.modify(itr, user, [&]( auto& address ) {
				address.city = city;
				address.state = "CA";
				address.zip = zip;
			});
			//这里事物回滚，所有操作将会执行失败
			eosio_assert(itr->city == "San", "Lock arf, Address not modified");
			//通过主键查询对象
			auto user = addresses.get(name("alice").value);
			eosio_assert(user.first_name == "Daniel", "Lock arf, Couldn't get him.");
			//返回索引
			auto zip_index = addresses.get_index<name("zip")>();
			auto itr = zip_index.lower_bound(zip);
			eosio_assert(itr->account_name == name("brendan"), "Lock arf, Incorrect First Lower Bound Record ");
			itr++;
			eosio_assert(itr->account_name == name("dan"), "Lock arf, Incorrect Second Lower Bound Record");
			itr++;
			eosio_assert(itr == zip_index.end(), "Lock arf, Incorrect End of Iterator");
		}
	}

	/**
	 *  cleos push action addressbook moreindex '["bob","guizhou",123]' -p bob
	 */
	[[eosio::action]]
	void moreindex(name user, string city, uint32_t zip) {
		//验证
		require_auth (user);
		address_index addresses(_self, _self.value); // code, scope
		// add to table, first argument is account to bill for storage
		//返回end时，表示没有找到相应的对象
		auto itr = addresses.find(user.value);
		if (itr == addresses.end()) {
			addresses.emplace(user, [&](auto& address) {
				address.account_name = user;
				address.first_name = "Daniel";
				address.last_name = "Larimer";
				address.street = "1 EOS Way";
				address.city = "Blacksburg";
				address.state = "VA";
				address.zip = zip;
			});
		} else {
			//_self:付款人是一个有效执行操作的有效帐户，需要为存储使用付费
			eosio_assert(itr != addresses.end(), "Lock arf, Address for account not found");
			addresses.modify(itr, user, [&]( auto& address ) {
				address.city = city;
				address.state = "CA";
				address.zip = zip;
			});
			//返回索引对象
			auto zip_index = addresses.get_index<name("zip")>();
			auto itr = zip_index.lower_bound(zip);
			itr++; //索引值加1
			eosio_assert(itr->account_name == name("bob"), "Lock arf, Incorrect First Lower Bound Record ");
			itr++; //索引值加1
			eosio_assert(itr->account_name == name("alice"), "Lock arf, Incorrect Second Lower Bound Record");
			itr++;
			eosio_assert(itr == zip_index.end(), "Lock arf, Incorrect End of Iterator");
			print("end");
		}
	}

	[[eosio::action]]
	void erasetable(name user) {
		//定义对象
		address_index addresses(_self, _self.value); // code, scope
		auto itr = addresses.find(user.value);
		//删除对象
		addresses.erase(itr);
		//表示没有对象删除
		eosio_assert(itr != addresses.end(), "Everting lock arf, Address not erased properly");
	}

	[[eosio::action]]
	void getfunction(name user) {
		// code, scope
		address_index addresses(name("code"), name("scope").value);
		//get_code:获取code名称
		print("get_code->", addresses.get_code());
		//需要转换成value值，不然是一个数字类型
		print("get_scope->", addresses.get_scope());
		eosio_assert(addresses.get_scope() == name("dan").value, "Lock arf, Scopes don't match");
		eosio_assert(addresses.get_code() == name("code"), "Lock arf, Codes don't match.");
	}
};

EOSIO_DISPATCH( addressbook, (myaction)(erasetable)(getfunction)(moreindex))
