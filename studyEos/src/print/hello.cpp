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
		require_auth(user);
		//打印输出指定的字符串。
		prints("Hello World!");
		//打印输出指定字符串的指定长度部分。
		prints_l("Hello World!", 5);
		//打印输出64位有符号整数。
		printi(-1e+18);
		//打印输出64位无符号整数。
		printui(1e+18);
		//打印输出128位有符号整数。
		int128_t large_int(-87654323456);
		printi128(&large_int); // Output: -87654323456
		//打印输出单精度浮点数，例如float类型的数。
		float valueF = 5.0 / 10.0;
		printsf(valueF); // Output: 0.5
		//打印输出双精度浮点数，例如double类型的数。
		double valueD = 5.0 / 10.0;
		printdf(valueD); // Output: 0.5
		//打印当前时间
		print(now());
	}
};
EOSIO_DISPATCH( hello, (hi)(typeprint))
