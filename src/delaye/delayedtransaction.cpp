/**
 * 发送延时交易：延时交易可以避免回滚攻击。
 *sudo eosio-cpp -o delayedtransaction.wasm delayedtransaction.cpp -abigen
 *cleos set contract delay /home/wxp/contracts/delayedtransaction
 *cleos push action delay delayedhi '["alice", 1]' -p alice
 *cleos push action delay hi '["alice", 1]' -p alice
 *cleos get table delay delay delay
 *
 *
 */
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/transaction.hpp>

using namespace eosio;

class [[eosio::contract]]delayedtransaction: public contract {
public:
	using contract::contract;

	delayedtransaction(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

	[[eosio::action]]
	void hi(name user, uint128_t id) {
		require_auth(user);
		print("Hello, ", name { user });
		eosio::transaction txn { };

		txn.actions.emplace_back(
				eosio::permission_level(_self, "active"_n),
				_self,
				"delayedhi"_n,
				std::make_tuple(user, id));
		txn.delay_sec = 1;
		//(sender_id, payer, replace_existed)
		txn.send(id, _self, false);
	}

	[[eosio::action]]
	void delayedhi(name user, uint128_t id) {
		printf("Delayed Hello %", id);
		id_index ids(_code, _code.value);
		auto iterator = ids.find(id);
		if (iterator == ids.end()) {
			ids.emplace(user, [&]( auto& row ) {
				row.user = user;
				row.id = id;
			});
		}
	}

	[[eosio::action]]
	void deletetable(name user,uint128_t id) {
		id_index ids(_code, _code.value);
		if (user.value != NULL) {
			// 寻找是否相同金额的下注方
			auto idx = ids.template get_index<"user"_n>();
			//表示查询最低值边界
			auto iterator = idx.find(user.value);
			eosio_assert(iterator != idx.end(), "user does not exist");
			idx.erase(iterator);
		} else {
			auto iterator = ids.find(id);
			eosio_assert(iterator != ids.end(), "id does not exist");
			ids.erase(iterator);
		}
	}

private:

	struct [[eosio::table]] delayed {
		name user;
		uint128_t id;
		uint128_t primary_key() const {
			return id;
		}
		uint64_t get_secondary_key() const {
			return user.value;
		}
	};
	// "delay"_n:delay(表名字)
	typedef eosio::multi_index<"delay"_n, delayed,
			indexed_by<"user"_n,const_mem_fun<delayed, uint64_t, &delayed::get_secondary_key>>> id_index;

};
EOSIO_DISPATCH( delayedtransaction, (hi)(delayedhi)(deletetable))
