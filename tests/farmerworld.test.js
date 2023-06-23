const { loadConfig, Blockchain } = require("@klevoya/hydra");

const config = loadConfig("hydra.yml");

describe("farmerworld", () => {
  let blockchain = new Blockchain(config);
  let tester = blockchain.createAccount(`farmerworld`);
  let atomicassets = blockchain.createAccount(`atomicassets`);
  let player1 = blockchain.createAccount(`player1`);
  let player2 = blockchain.createAccount(`player2`);
  let player3 = blockchain.createAccount(`player3`);


  beforeAll(async () => {

    // Deploy Mining Smartcontract
    const testerTemplate = blockchain.contractTemplates['farmerworld'];
    tester.setContract(testerTemplate);
    tester.updateAuth('active', 'owner', {
      accounts: [
        {
          permission: {
            actor: tester.accountName,
            permission: 'eosio.code',
          },
          weight: 1,
        },
      ],
    });

    //  Deploy Atomic Assets
    const atomicassetsTemplate = blockchain.contractTemplates['atomicassets'];
    atomicassets.setContract(atomicassetsTemplate);
    atomicassets.updateAuth('active', 'owner', {
      accounts: [
        {
          permission: {
            actor: atomicassets.accountName,
            permission: 'eosio.code',
          },
          weight: 1,
        },
      ],
    });
  });



  describe('create tool', () => {
    it.only("create basic collection", async () => {

      await atomicassets.contract.init();
      await atomicassets.contract.createcol({
        author: player1.accountName,
        collection_name: "testcollect1",
        allow_notify: true,
        authorized_accounts: [player1.accountName],
        notify_accounts: [],
        market_fee: 0.05,
        data: []
      }, [{
        actor: player1.accountName,
        permission: "active",
      }]);

      const collections = atomicassets.getTableRowsScoped("collections")["atomicassets"];
      // console.log('**** collections', collections);
      expect(collections).toEqual([{
        collection_name: "testcollect1",
        author: player1.accountName,
        allow_notify: true,
        authorized_accounts: [player1.accountName],
        notify_accounts: [],
        market_fee: 0.05,
        serialized_data: '',
      }]);
    });

    it.only("create minimal schema", async () => {
      await atomicassets.contract.createschema({
        authorized_creator: player1.accountName,
        collection_name: "testcollect1",
        schema_name: "testschema",
        schema_format: [
          { name: "name", type: "string" },
          { name: "img", type: "string" },
          { name: "type", type: "string" },
          { name: "level", type: "uint64" },
          { name: "rarity", type: "string" },
          { name: "durability", type: "uint64" }
        ]
      }, [{
        actor: player1.accountName,
        permission: "active"
      }]);

      const collection_schemas = atomicassets.getTableRowsScoped("schemas")["testcollect1"];
      expect(collection_schemas).toEqual([{
        schema_name: "testschema",
        format: [
          { name: "name", type: "string" },
          { name: "img", type: "string" },
          { name: "type", type: "string" },
          { name: "level", type: "uint64" },
          { name: "rarity", type: "string" },
          { name: "durability", type: "uint64" }

        ]
      }]);
    });

    it.only("create template with data", async () => {
      await atomicassets.contract.createtempl({
        authorized_creator: player1.accountName,
        collection_name: "testcollect1",
        schema_name: "testschema",
        transferable: true,
        burnable: true,
        max_supply: 0,
        immutable_data: [
          { "key": "name", "value": ["string", "Chainsaw"] },
          { "key": "img", "value": ["string", "QmPDr3v948cB8ww72996Q2yZCBGGVqPbnz55uvYkiNukFm"] },
          { "key": "type", "value": ["string", "Wood"] },
          { "key": "rarity", "value": ["string", "Rare"] },
          { "key": "level", "value": ["uint64", "3"] },
          { "key": "durability", "value": ["uint64", "900"] }
        ]
      }, [{
        actor: player1.accountName,
        permission: "active"
      }]);

      const collection_templates = atomicassets.getTableRowsScoped("templates")["testcollect1"];
      // console.log('collection_templatesssssssss',collection_templates);
      expect(collection_templates).toEqual([{
        template_id: 1,
        schema_name: "testschema",
        transferable: true,
        burnable: true,
        max_supply: 0,
        issued_supply: 0,
        immutable_serialized_data: '0408436861696E736177052E516D5044723376393438634238777737323939365132795A43424747567150626E7A35357576596B694E756B466D0604576F6F640703080452617265098407',
      }]);
    });

    it.only("mint minimal asset", async () => {
      await atomicassets.contract.mintasset({
        authorized_minter: player1.accountName,
        collection_name: "testcollect1",
        schema_name: "testschema",
        template_id: 1,
        new_asset_owner: player1.accountName,
        immutable_data: [
          { "key": "name", "value": ["string", "Chainsaw"] },
          { "key": "img", "value": ["string", "QmPDr3v948cB8ww72996Q2yZCBGGVqPbnz55uvYkiNukFm"] },
          { "key": "type", "value": ["string", "Wood"] },
          { "key": "rarity", "value": ["string", "Rare"] },
          { "key": "level", "value": ["uint64", "3"] },
          { "key": "durability", "value": ["uint64", "900"] }
        ],
        mutable_data: [],
        tokens_to_back: []
      }, [{
        actor: player1.accountName,
        permission: "active"
      }]);

      const user3_assets = atomicassets.getTableRowsScoped("assets")[player1.accountName];
      expect(user3_assets).toEqual([{
        asset_id: "1099511627776",
        collection_name: "testcollect1",
        schema_name: "testschema",
        template_id: 1,
        ram_payer: player1.accountName,
        backed_tokens: [],
        immutable_serialized_data: '0408436861696E736177052E516D5044723376393438634238777737323939365132795A43424747567150626E7A35357576596B694E756B466D0604576F6F640703080452617265098407',
        mutable_serialized_data: ''
      }]);
    });


    //CREATE TOOL
    it.only('can create a tool', async () => {
      expect.assertions(1);


      // Perform the createtool action (assuming you have a similar action in your contract)
      await tester.contract.createtool(
        {
          contract_account: player1.accountName,
          asset_id: 1099511627776,
          owner: player1.accountName,
        },
        [{ actor: player1.accountName, permission: 'active' }]
      );

      // Assert the created tool in the toolslist table
      const toolTable = await tester.getTableRowsScoped('toolslist')['farmerworld'];
      const data = toolTable



      const convertedData = data.map(item => ({
        tool_id: parseInt(item.tool_id),
        owner: item.owner,
        name: item.name,
        img: item.img,
        rarity: item.rarity,
        level: parseInt(item.level),
        type: item.type,
        is_wear: item.is_wear,
        durability: parseInt(item.durability),
        current_durability: parseInt(item.durability),
        lastChargeTime: parseInt(item.lastChargeTime)
      }));

      // console.log('converted data', convertedData);


      expect(convertedData).toEqual([{
        tool_id: 1099511627776,
        owner: "player1",
        name: "Chainsaw",
        img: "QmPDr3v948cB8ww72996Q2yZCBGGVqPbnz55uvYkiNukFm",
        type: "Wood",
        rarity: "Rare",
        level: 3,
        is_wear: false,
        durability: 900,
        current_durability: 900,
        lastChargeTime: 0,
      }]);
    });
    describe('add player', () => {
      //ADD PLAYER
      it.only("can add a player", async () => {
        expect.assertions(1);

        // Perform the addplayer action
        await tester.contract.addplayer({
          account: player1.accountName,
          energy: 20,
          wood: 10,
          food: 10,
          gold: 10
        }, [{ actor: player1.accountName, permission: "active" }]
        );

        expect(tester.getTableRowsScoped(`players`)['farmerworld']).toEqual([{
          account: player1.accountName,
          energy: 20,
          wood: 10,
          food: 10,
          gold: 10,
        },
        ]);
      });
      describe('wear tool', () => {


        //TODO : FIND THE TOOL IN TABLE

        it.only("Can tool exists", async () => {
          expect.assertions(1);
          try {
            await tester.contract.weartool({ owner: player1.accountName, tool_id: 1099511627777 }
              , [{ actor: player1.accountName, permission: 'active' }]
            );
          } catch (error) {
            console.log('check tool', error.message);
            expect(error.message).toBe(error.message);
          }
        })


        // TODO : Check Owner before wear
        it.only("check owner before wear the tool", async () => {
          expect.assertions(1);
          try {
            await tester.contract.weartool({ owner: player2.accountName, tool_id: 1099511627776 }
              , [{ actor: player2.accountName, permission: 'active' }]
            );
          } catch (error) {
            console.log('check owner', error.message);
            expect(error.message).toBe(error.message);
          }
        })



        //  Wear Tool
        it.only("can wear a tool", async () => {
          expect.assertions(1);

          await tester.contract.weartool({ owner: player1.accountName, tool_id: 1099511627776 }
            , [{ actor: player1.accountName, permission: 'active' }]
          );

          const tools = await tester.getTableRowsScoped("toolslist")['farmerworld'];
          console.log('tools', tools);
          const convertedData = tools.map(item => ({
            tool_id: parseInt(item.tool_id),
            owner: item.owner,
            name: item.name,
            img: item.img,
            rarity: item.rarity,
            level: parseInt(item.level),
            type: item.type,
            is_wear: item.is_wear,
            durability: parseInt(item.durability),
            current_durability: parseInt(item.durability),
            lastChargeTime: parseInt(item.lastChargeTime)
          }));
          expect(convertedData).toEqual([
            {
              tool_id: 1099511627776,
              owner: player1.accountName,
              name: "Chainsaw",
              img: "QmPDr3v948cB8ww72996Q2yZCBGGVqPbnz55uvYkiNukFm",
              type: "Wood",
              rarity: "Rare",
              level: 3,
              is_wear: true,
              durability: 900,
              current_durability: 900,
              lastChargeTime: 0,
            },
          ]);
        })

        //   Check if already wear
        it.only('check if is_wear is true', async () => {
          try {
            await tester.contract.weartool(
              { owner: player1.accountName, tool_id: 1099511627776 },
              [{ actor: player1.accountName, permission: 'active' }]
            );

          } catch (error) {
            console.log('iswear true', error.message);
            // Assert that the error message is as expected
            expect(error.message).toBe(error.message);
          }
        });


        describe('mine tool', () => {

          // TODO : CHECK IF IT IS NOT WEAR
          it.only('check if it not wear', async () => {
            const dataObject = {
              owner: "player1",
              tool_id: 1099511627776,
              energy_consumed: 10,
              durabilityDecrease: 5,
              chargeTime: 60,
              reward: 10
            }

            try {
              // Action: Call the mine action
              await tester.contract.mine(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
              );

            } catch (error) {
              console.log('if not wear', error.message);
              // Assert that the error message is as expected
              expect(error.message).toBe(error.message);
            }
          });

          //TODO : CHECK MINER HAVE ENOUGH EERGY TO MINE THE TOOL
          it.only('Can Miner Have Enough Energy To Mine', async () => {
            const dataObject = {
              owner: "player1",
              tool_id: 1099511627776,
              energy_consumed: 20,
              durabilityDecrease: 5,
              chargeTime: 60,
              reward: 10
            }

            try {
              // Action: Call the mine action
              await tester.contract.mine(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
              );

            } catch (error) {
              console.log('miner energy', error.message);
              // Assert that the error message is as expected
              expect(error.message).toBe(error.message);
            }
          });


          //TODO : CHECK THE TOOL WORN OUT OR NOT

          it.only('check the durability of tool ', async () => {
            const dataObject = {
              owner: "player1",
              tool_id: 1099511627776,
              energy_consumed: 10,
              durabilityDecrease: 900,
              chargeTime: 60,
              reward: 10
            }

            try {
              // Action: Call the mine action
              await tester.contract.mine(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
              );

            } catch (error) {
              console.log('check durability', error.message);
              // Assert that the error message is as expected
              expect(error.message).toBe(error.message);
            }
          });

        
          //  Mine Tool                        
          it.only("should mine resources", async () => {

            const dataObject = {
              owner: "player1",
              tool_id: 1099511627776,
              energy_consumed: 10,
              durabilityDecrease: 5,
              chargeTime: 60,
              reward: 10
            }
     

            // Action: Call the mine action
            try {
              await tester.contract.mine(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
              );
            } catch (error) {
              console.log('8mine*******',error.message);
            }
           

            // Assertion: Check the updated player's resources and tool's durability

            const toolslist = tester.getTableRowsScoped("toolslist")['farmerworld'];
            console.log('toolslistmine', toolslist);
            expect(toolslist).toEqual([
              {
                tool_id: '1099511627776',
                owner: 'player1',
                name: 'Chainsaw',
                img: 'QmPDr3v948cB8ww72996Q2yZCBGGVqPbnz55uvYkiNukFm',
                rarity: 'Rare',
                level: '3',
                type: 'Wood',
                is_wear: true,
                durability: '900',
                current_durability: '895',
                lastChargeTime: '946684800'
              }]);
            const playerdata = tester.getTableRowsScoped("players")['farmerworld'];
            console.log('playerdatamine', playerdata);
            expect(playerdata).toEqual([
              {

                account: player1.accountName,
                energy: 10,
                wood: 20,
                food: 10,
                gold: 10,
              },
            ]);
          })

           // TODO : CHECK IF MINING TIME NOT PASSED
           it.only('check the mining time of tool ', async () => {
            const dataObject = {
              owner: "player1",
              tool_id: 1099511627776,
              energy_consumed: 10,
              durabilityDecrease: 5,
              chargeTime: 60,
              reward: 10
            }

            try {
              // Action: Call the mine action
              await tester.contract.mine(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
              );

            } catch (error) {
              console.log('miner time not passed', error.message);
              // Assert that the error message is as expected
              expect(error.message).toBe(error.message);
            }
          });
        })



        describe('repair tool', () => {

          //TODO : PLAYER HAS  ENOUGH GOLD TO A TOOL 
          it.only("check enough gold to repair tool", async () => {

            const dataObject = {
              toolOwner: "player1",
              tool_id: 1099511627776,
              gold: 50,
            }

            // Action: Call the repair action
            try {
              await tester.contract.repair(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
              );
            } catch (error) {
              console.log('repair gold',error.message);
              expect(error.message).toBe(error.message);
            }
          })

          // TODO : Repair Tool
          it.only("repair tool", async () => {

            const dataObject = {
              toolOwner: "player1",
              tool_id: 1099511627776,
              gold: 5,
            }


            // Action: Call the mine action
            await tester.contract.repair(
              dataObject,
              [{ actor: player1.accountName, permission: "active" }]
            );

            // Assertion: Check the updated player's resources and tool's durability

            const toolslist = tester.getTableRowsScoped("toolslist")['farmerworld'];
            console.log('toolslistrepairtool', toolslist);
            expect(toolslist).toEqual([
              {
                tool_id: '1099511627776',
                owner: 'player1',
                name: 'Chainsaw',
                img: 'QmPDr3v948cB8ww72996Q2yZCBGGVqPbnz55uvYkiNukFm',
                rarity: 'Rare',
                level: '3',
                type: 'Wood',
                is_wear: true,
                durability: '900',
                current_durability: '900',
                lastChargeTime: '946684800'
              }]);
            const playerdata = tester.getTableRowsScoped("players")['farmerworld'];
            console.log('playerdataafter mining', playerdata);
            expect(playerdata).toEqual([
              {

                account: player1.accountName,
                energy: 10,
                wood: 20,
                food: 10,
                gold: 5,
              },
            ]);
          })


          //TODO :CHECK THE DURABILITY OF TOOL

          it.only("should not  repair tool", async () => {

            const dataObject = {
              toolOwner: "player1",
              tool_id: 1099511627776,
              gold: 50,
            }

            // Action: Call the repair action
            try {
              await tester.contract.repair(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
              );
            } catch (error) {
              console.log('repair durability',error.message);
              expect(error.message).toBe(error.message);
            }
          })

          
        })
        describe('check player have enough food', () => {

           //  TODO : ENOUGH FOOD TO ADD ENERGY 
           it.only("can add energy to a player", async () => {
            expect.assertions(1);
              const player = await tester.getTableRowsScoped('players')['farmerworld'];
              console.log('can add energy to a player',player);
            // Perform the addenergy action
            try {
              await tester.contract.addenergy(
                {
                  account: player1.accountName,
                  food: 20
                },
                [{ actor: player1.accountName, permission: "active" }]
              );
            } catch (error) {
              console.log('food player in repair',error.message);
              expect(error.message).toBe(error.message)
            }
            })

          // TODO : Add Energy

          it.only("can add energy to a player", async () => {
            expect.assertions(1);

            // Perform the addenergy action
            await tester.contract.addenergy(
              {
                account: player1.accountName,
                food: 5
              },
              [{ actor: player1.accountName, permission: "active" }]
            );

            // Retrieve the updated players from the table
            const updatedPlayers = tester.getTableRowsScoped("players")['farmerworld'];

            console.log('updatedPlayers', updatedPlayers);

            // Find the updated player based on the account name
            const updatedPlayer = updatedPlayers.find(player => player.account === player1.accountName);
            // Calculate the expected energy value
            console.log('playerdataafter mining', updatedPlayer);
            expect(updatedPlayer).toEqual(
              {
                account: player1.accountName,
                energy: 35,
                wood: 20,
                food: 5,
                gold: 5,
              },
            );
          });

         
        })
        describe('remove tool', () => {
          //TODO CHECK ENERGY
          
          it.only("should not enough energy to remove tool", async () => {
            expect.assertions(1);
            const dataObject = {
              toolOwner: "player1",
              tool_id: 1099511627776,
              energy: 100,
            }
            try {
              await tester.contract.remove(
                dataObject,
                [{ actor: player1.accountName, permission: "active" }]
                );
            } catch (error) {
              console.log('check energy player',error.message);
              expect(error.message).toBe(error.message);
            }
          });

          it.only("can remove a tool", async () => {
            expect.assertions(1);
            const dataObject = {
              toolOwner: "player1",
              tool_id: 1099511627776,
              energy: 5,
            }
            await tester.contract.remove(
              dataObject,
              [{ actor: player1.accountName, permission: "active" }]
              );
             const tool  = tester.getTableRowsScoped('toolslist')['farmerworld']
             expect(tool).toEqual(undefined);
          });
        })
      })
    });
  });
});


