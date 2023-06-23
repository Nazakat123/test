#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include "atomicassets-interface.hpp"
#include "atomicdata.hpp"
#include "checkformat.hpp"



using namespace atomicassets;
using namespace atomicdata;

using namespace eosio;
using namespace std;

CONTRACT farmerworld : public contract {
public:
    using contract::contract;

   // Define the Tool table
    TABLE tool {
        uint64_t tool_id;
        name owner;         
        string name;
        string img;
        string rarity;
        uint64_t level;
        string type;
        bool is_wear = false ; 
        uint64_t durability;
        uint64_t current_durability;
        uint64_t lastChargeTime;


        uint64_t primary_key() const { return tool_id; }

    };

    typedef eosio::multi_index<"toolslist"_n, tool> tool_table;

struct [[eosio::table]] player {
    name account;           // Account name of the player
    uint32_t energy;        // Energy owned  of the player
    uint32_t wood;          // Amount of wood owned by the player
    uint32_t food;          // Amount of food owned by the player
    uint32_t gold;          // Amount of gold owned by the player
   
    // Specify the primary key of the table
    uint64_t primary_key() const { return account.value; }
};

    // Define the player table
   typedef eosio::multi_index<"players"_n, player> table_players;


    [[eosio::action]]
    void addplayer( name account,  uint32_t energy,  uint32_t wood,  uint32_t food,   uint32_t gold ){
        require_auth(account);

       table_players players(get_self(), get_self().value);


        players.emplace(account, [&](auto& p) {  
            p.account = account ;
            p.energy = energy ;
            p.wood = wood ;
            p.food = food ;
            p.gold = gold ;
        });
    }


    [[eosio::action]]
    void removeplayer( name account ){
        require_auth(account);

       table_players players(get_self(), get_self().value);

       
        auto iterator = players.find(account.value);

        check(iterator->account == account, "Player not Found");

        players.erase(iterator);
    }

ACTION createtool(name contract_account, uint64_t asset_id, name owner) {
    require_auth(owner);

    // Get table
    assets_t assets = get_assets(contract_account);
    auto iter = assets.find(asset_id);

    schemas_t collection_schemas = get_schemas(iter->collection_name);
    auto schema_itr = collection_schemas.require_find(iter->schema_name.value, "No schema with this name exists");

    templates_t collection_templates = get_templates(iter->collection_name);
    auto template_itr = collection_templates.require_find(iter->template_id, "No template with this id exists");

    ATTRIBUTE_MAP deserialized_template_data;
    deserialized_template_data = deserialize(template_itr->immutable_serialized_data, schema_itr->format);

    tool_table toolslist(get_self(), get_self().value);

    tool toolAttributes;
    for (const auto& attribute : deserialized_template_data) {
        const std::string& key = attribute.first;
        const auto& value = attribute.second;

        if (key == "name") {
            std::visit([&toolAttributes](const auto& v) {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                    toolAttributes.name = v;
                }
            }, value);
        } else if (key == "img") {
            std::visit([&toolAttributes](const auto& v) {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                    toolAttributes.img = v;
                }
            }, value);
        } else if (key == "durability") {
            std::visit([&toolAttributes](const auto& v) {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, uint64_t>) {
                    toolAttributes.durability = v;
                }
            }, value);
        } else if (key == "type") {
            std::visit([&toolAttributes](const auto& v) {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                    toolAttributes.type = v;
                }
            }, value);
        } else if (key == "rarity") {
            std::visit([&toolAttributes](const auto& v) {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                    toolAttributes.rarity = v;
                }
            }, value);
           } else if (key == "level") {
            std::visit([&toolAttributes](const auto& v) {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, uint64_t>) {
                    toolAttributes.level = v;
                }
            }, value);
            }
    
    }
    // Create the new tool
    toolslist.emplace(owner, [&](auto& tool) {
        tool.tool_id = iter->asset_id;
        tool.owner = owner;
        tool.name = toolAttributes.name;
        tool.img = toolAttributes.img; 
        tool.type = toolAttributes.type;
        tool.rarity = toolAttributes.rarity;
        tool.level = toolAttributes.level;
        tool.durability = toolAttributes.durability;
        tool.current_durability = toolAttributes.durability; 
        tool.lastChargeTime = 0; 
    });

}
    
  

    ACTION weartool(name owner, uint64_t tool_id) {
        require_auth(owner);

        tool_table toolslist(get_self(), get_self().value);
        auto iterator = toolslist.find(tool_id);

        check(iterator != toolslist.end(), "Tool not found.");

        check(iterator->owner == owner, "You do not own this tool.");

        check(iterator->is_wear == false, "You have already wear the tool");

        // set bool to true
        toolslist.modify(iterator,owner,[&](auto& tool){
            tool.is_wear = true ;
        });
    }

    ACTION mine(name owner , uint64_t tool_id ,uint64_t energy_consumed , uint64_t durabilityDecrease , uint64_t reward , uint32_t chargeTime) {
        require_auth(owner);

        tool_table toolslist(get_self(), get_self().value);
        auto iterator = toolslist.find(tool_id);

        check(iterator != toolslist.end(), "Tool not found.");

        check(iterator->owner == owner, "You do not own this tool.");

        check(iterator->is_wear == true , "Please first wear the tool and then mine it");

        table_players players(get_self(), get_self().value);
        auto itr = players.find(owner.value);
   

        check(itr != players.end(), "Miner's energy not found."); // Check if the energy record exists

    
         // check if the energy is above a certain threshold required for mining
        check(itr->energy >= energy_consumed, "Miner doesn't have enough energy to mine.");



        check(iterator->current_durability >= durabilityDecrease, "Miner's tool is worn out and cannot be used for mining.");


        uint32_t currentTimestamp = current_time_point().sec_since_epoch();
        uint32_t  totalchargeTime = chargeTime * 60  ;

       //check mining time passed
       check(iterator->lastChargeTime == 0 || (currentTimestamp - iterator->lastChargeTime >= totalchargeTime), "Not enough charge time has passed. Try again later.");


         uint32_t minedResources = performMiningLogic(reward);

        //update resources and decrease energy
       if (iterator -> type  == "Wood") {
         players.modify(itr, owner, [&](auto& p) {
        p.energy -= energy_consumed ; 
        p.wood += minedResources;
        });
        } else if (iterator -> type  == "Food") {
        players.modify(itr, owner, [&](auto& p) {
        p.energy -= energy_consumed ; 
        p.food += minedResources;
        });
        } else if (iterator -> type  == "Gold") {
        players.modify(itr, owner, [&](auto& p) {
        p.energy -= energy_consumed ; 
        p.gold += minedResources;
        });
        }

        //update current time and decrease durability
        toolslist.modify(iterator, owner, [&](auto& p) {
                p.lastChargeTime = currentTimestamp;
                p.current_durability -= durabilityDecrease;
        });
        
    }


   uint32_t performMiningLogic( uint64_t reward) {
    // Return the quantity of mined resources
    uint32_t minedResources = reward; 
    return minedResources;
}

 ACTION addenergy(name account , uint32_t food){
     require_auth(account);
     table_players players(get_self(),get_self().value);
     auto iterator = players.find(account.value);

     check(iterator != players.end(),"player not exists");  //TODO
     
    check (iterator->food >= food , "Not Enough Food");

     uint32_t energy = 5 * food ;

     players.modify(iterator,account,[&](auto& p){
         p.energy += energy ;
         p.food -= food ;
     });
 }



    ACTION repair(name toolOwner, uint64_t tool_id ,  uint32_t gold) {
        require_auth(toolOwner);

        // get table 

        tool_table toolslist(get_self(), get_self().value);

        // Find the tool with the specified toolId
        auto tool = toolslist.find(tool_id);
        check(tool != toolslist.end(), "Tool not found.");

        // Check the durability of tool
        check(tool->current_durability != tool->durability , "Tool does not require repair.");

        //get table  
        table_players players(get_self(), get_self().value);

        // Find the player
        auto iterator = players.find(toolOwner.value);
        check(iterator != players.end(), "Player not found");

        //check player has enough gold to repair the tool
        check(iterator->gold >= gold , "Not enough gold to repair");

        

        // repair operation
        uint32_t repairAmount = calculateRepairAmount(tool->durability, tool->current_durability);

        // Update the tool's durability
        toolslist.modify(tool, toolOwner, [&](auto& t) {
            t.current_durability += repairAmount;
            if (t.current_durability > t.durability) {
                t.current_durability = t.durability;
            }
        });

         // Update the player resources
         players.modify(iterator,toolOwner, [&](auto& p){
                p.gold -= gold ;
        });
    }

   
    ACTION remove(name toolOwner, uint64_t tool_id , uint32_t energy) {
        require_auth(toolOwner);

          //get table  
        table_players players(get_self(), get_self().value);

        // Find the tool owner
        auto iterator = players.find(toolOwner.value);
        check(iterator != players.end(), "Player not found");

        check (iterator->energy >= energy , "Not enough energy to remove the tool");  //again check

        players.modify(iterator,toolOwner,[&](auto& p){
            p.energy -= energy ; 
        });

        // get table

        tool_table toolslist(get_self(), get_self().value);

        // Find the tool with the specified toolId
        auto tool = toolslist.find(tool_id);
        check(tool != toolslist.end(), "Tool not found.");

        // Check if the tool belongs to the owner
        check(tool->owner == toolOwner, "You only remove your own tool.");

        // Erase the tool from the table
        toolslist.erase(tool);

    }

private:
    uint32_t calculateRepairAmount(uint32_t maxDurability, uint32_t currentDurability) {
        // Calculate the repair amount based on the difference between maxDurability and currentDurability
        uint32_t repairAmount = maxDurability - currentDurability;
        return repairAmount;
    }
};

EOSIO_DISPATCH(farmerworld, (addplayer)(removeplayer)(createtool)(weartool)(mine)(addenergy)(repair)(remove))




//   it("can remove a player", async () => {
//     expect.assertions(1);

//     await tester.contract.removeplayer(player1.accountName);
//     const players = await tester.getTableRowsScoped("farmerworld", tester.accountName, "players");
//     expect(players).toEqual([]);
//   });

//   it("can create a tool", async () => {
//     expect.assertions(1);

//     await tester.contract.addplayer(player1.accountName, 100, 10, 20, 30);
//     await tester.contract.createtool(tester.accountName, 1, player1.accountName);

//     const tools = await tester.getTableRowsScoped("farmerworld", tester.accountName, "toolslist");
//     expect(tools).toEqual([
//       {
//         tool_id: 1,
//         owner: player1.accountName,
//         name: "",
//         img: "",
//         rarity: "",
//         level: 0,
//         type: "",
//         is_wear: false,
//         durability: 0,
//         current_durability: 0,
//         lastChargeTime: 0,
//       },
//     ]);
//   });

//   it("can wear a tool", async () => {
//     expect.assertions(1);

//     await tester.contract.weartool(player1.accountName, 1);

//     const tools = await tester.getTableRowsScoped("farmerworld", tester.accountName, "toolslist");
//     expect(tools).toEqual([
//       {
//         tool_id: 1,
//         owner: player1.accountName,
//         name: "",
//         img: "",
//         rarity: "",
//         level: 0,
//         type: "",
//         is_wear: true,
//         durability: 0,
//         current_durability: 0,
//         lastChargeTime: 0,
//       },
//     ]);
//   })


  // it("can remove a player", async () => {


  //   // Perform the removeplayer action
  //   await tester.contract.removeplayer({ account: player1.accountName }, [{ actor: player1.accountName, permission: "active" }]);

  //   // Check if the player is no longer in the table
  //   expect(tester.getTableRowsScoped(`players`)['farmerworld']).toBeUndefined();

  // });

  // it("can create a tool", async () => {
  //   expect.assertions(1);

  //   await tester.contract.addplayer({ account: player1.accountName, energy: 100, wood: 10, food: 20, gold: 30 }, [{ actor: player1.accountName, permission: "active" }]);
  //   await tester.contract.createtool({contract_account : player2.accountName,asset_id : 1099557743634, owner : player1.accountName},[{ actor: player1.accountName, permission: "active" }]);

  //   const tools =  tester.getTableRowsScoped(`toolslist`)['farmerworld'];
  //   expect(tools).toEqual([
  //     {
  //       tool_id: 1099557743634,
  //       owner: player1.accountName,
  //       name: "",
  //       img: "",
  //       rarity: "",
  //       level: 0,
  //       type: "",
  //       is_wear: false,
  //       durability: 0,
  //       current_durability: 0,
  //       lastChargeTime: 0,
  //     },
  //   ]);
  // });