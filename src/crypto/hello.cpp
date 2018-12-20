/**
 * sudo eosio-cpp -o hello.wasm hello.cpp -abigen
 * cleos set  contract bob /home/wxp/contracts/hello
 * cleos push action bob typeprint '["alice"]' -p alice
 *
 *
 */
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;
/**
 *
 */
class hello: public contract {
public:
	using contract::contract;

	//Test print
	[[eosio::action]]
	void hi(name user, asset a) {
		require_auth(user);
		std::string msg = "Our world is beautiful";
		//打印字符串
		prints("Hello World!");
		prints_l("Hello World!", 5); // Output: Hello
		//为假时中断
		eosio_assert(1 == 1, "One is equal to two.");
	}
	//Test print
	[[eosio::action]]
	void typeprint(name user) {
	}
};
EOSIO_DISPATCH( hello, (hi)(typeprint))
