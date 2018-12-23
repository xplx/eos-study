#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract]] luckpoint: public eosio::contract {

public:
	using contract::contract;

	luckpoint(name receiver, name code, datastream<const char*> ds) :
			contract(receiver, code, ds) {
	}

	[[eosio::action]]
	void creategame(name owner) {
		global_index globals(_code, _code.value);
		game_index games(_code, _code.value);
		// 创建一个全局引用计数器
		auto global_itr = globals.begin();
		if (global_itr == globals.end()) {
			global_itr = globals.emplace(owner, [&](auto& row) {
				row.nextgameid=0;
			});
		}
		// 增量引用计数器
		globals.modify(global_itr, owner, [&](auto& row) {
			row.nextgameid++;
		});

		// 创建一个新游戏对象
		auto game_itr = games.emplace(owner, [&](auto& new_game) {
			new_game.id = global_itr->nextgameid;
			new_game.winner = 0;
			new_game.player1 = 0;
			new_game.player2 = 0;
			new_game.createtime = now();
			new_game.owner = owner;
		});
	}

	/**
	 * 开牌
	 */
	[[eosio::action]]
	void opencard(uint64_t _gameid, const uint32_t _player, name owner) {
		game_index games(_code, _code.value);
		eosio_assert(_player == 1 || _player == 2, "the player must is 1 or 2.");
		auto game_itr = games.find(_gameid);
		eosio_assert(game_itr != games.end(), "game not found");

		// 修改游戏数据状态
		games.modify(game_itr, owner, [&](auto& gitr) {
			if(_player == 1) {
				gitr.player1 = 1;
			} else if(_player == 2) {
				gitr.player2 = 1;
			}
			// 检查游戏是否已经决出胜出者
			gitr.winner = _checkgame(_gameid, owner);
			});
	}

	/**
	 * 检查游戏
	 */
	uint32_t _checkgame(uint64_t _gameid, name owner) {
		game_index games(_code, _code.value);
		auto game_itr = games.find(_gameid);
		eosio_assert(game_itr != games.end(), "game not found");

		if (game_itr->player1 != 0 && game_itr->player2 != 0) {
			eosio::print("modify winner");
			return _getrandnum(_gameid);
		}
		return 0;
	}

	uint32_t _getrandnum(uint64_t _gameid) {
		game_index games(_code, _code.value);
		auto game_itr = games.find(_gameid);
		eosio_assert(game_itr != games.end(), "game id not found in games");

		// 随机数=游戏创建时间，取余2，再+1，保证是1、2中的任意一个
		return (((uint32_t) (game_itr->createtime)) % 2) + 1;
	}

	/**
	 * 打印随机数
	 */
	[[eosio::action]]
	void printrand(uint64_t _gameid) {
	  eosio::print("printrand::%d", _getrandnum(_gameid));
	}

//表结构定义
private:
	//table global i64 全局引用计数表
	struct [[eosio::table]] global {
		uint64_t id = 0;
		uint64_t nextgameid = 0;
		uint64_t primary_key() const {
			return id;
		}
		EOSLIB_SERIALIZE( global, (id)(nextgameid) )};

		// 创建一个多索引容器的游戏引用计数，用于为新游戏生成游戏ID
		typedef eosio::multi_index<"global"_n, global> global_index;

		//table game i64 游戏表
		struct [[eosio::table]] game {
			uint64_t id;           // 游戏ID
			uint32_t winner;// 胜出者，0还未决出胜负，1玩家1胜出，2玩家2胜出
			uint32_t player1;// 玩家1点数
			uint32_t player2;// 玩家2点数
			uint32_t createtime;// 创建时间
			name owner;// 创建游戏的账户
			uint64_t primary_key()const {return id;}  // 设置数据主键
			EOSLIB_SERIALIZE( game, (id)(winner)(player1)(player2)(createtime)(owner) )
		};

		// 创建一个多索引容器的游戏列表
		typedef eosio::multi_index< "game"_n, game> game_index;
	};

//这里一定要添加对应的方法，不然调用合约无效
EOSIO_DISPATCH( luckpoint, (creategame)(opencard)(printrand))


