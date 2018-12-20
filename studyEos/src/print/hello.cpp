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
		//��ӡ�ַ���
		prints("Hello World!");
		prints_l("Hello World!", 5); // Output: Hello
		//Ϊ��ʱ�ж�
		eosio_assert(1 == 1, "One is equal to two.");
	}
	//Test print
	[[eosio::action]]
	void typeprint(name user) {
		require_auth(user);
		//��ӡ���ָ�����ַ�����
		prints("Hello World!");
		//��ӡ���ָ���ַ�����ָ�����Ȳ��֡�
		prints_l("Hello World!", 5);
		//��ӡ���64λ�з���������
		printi(-1e+18);
		//��ӡ���64λ�޷���������
		printui(1e+18);
		//��ӡ���128λ�з���������
		int128_t large_int(-87654323456);
		printi128(&large_int); // Output: -87654323456
		//��ӡ��������ȸ�����������float���͵�����
		float valueF = 5.0 / 10.0;
		printsf(valueF); // Output: 0.5
		//��ӡ���˫���ȸ�����������double���͵�����
		double valueD = 5.0 / 10.0;
		printdf(valueD); // Output: 0.5
		//��ӡ��ǰʱ��
		print(now());
	}
};
EOSIO_DISPATCH( hello, (hi)(typeprint))
