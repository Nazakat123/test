const { loadConfig, Blockchain ,contractMocks} = require("@klevoya/hydra");

const config = loadConfig("hydra.yml");
const fs = require('fs');

describe("farmerworld", () => {
  let blockchain = new Blockchain(config);
  let tester = blockchain.createAccount(`farmerworld`);
  // let atomicassets = blockchain.createAccount(`atomicassets`);

  let player1 = blockchain.createAccount(`player1`);
  let player2 = blockchain.createAccount(`player2`);
  let player3 = blockchain.createAccount(`player3`);

  beforeAll(async () => {
    // TODO: Deploy Atomic Assets
  
    // TODO: Create collection and schemas
  
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

  beforeAll(async () => {

    //TODO: Deploy Atomic Assets
  
  

    //TODO: Create collection and schemas

    // Deploy Mining Smartcontract
    // atomicassets.setContract(blockchain.contractTemplates[`atomicassets`]);
  //   player2.setContract(blockchain.contractTemplates[`nazakat12345`]);
  //   tester.updateAuth(`active`, `owner`, {
  //     accounts: [
  //       {
  //         permission: {
  //           actor: tester.accountName,
  //           permission: `eosio.code`,
  //         },
  //         weight: 1,
  //       },
  //     ],
  //   });
    //   // Set up tester contract template
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
  });

  // });
 

  // beforeAll(async () => {
   
  //   const atomicassetsTemplate = await tester.deployContract("atomicassets", fs.readFileSync("/home/nazakat/tutorial_contracts/atomicassets-contract-master/src/atomicassets.wasm"), fs.readFileSync("/home/nazakat/tutorial_contracts/atomicassets-contract-master/src/atomicassets.abi"));
  // atomicassets.setContract(atomicassetsTemplate);
  // atomicassets.updateAuth("active", "owner", {
  //   accounts: [
  //     {
  //       permission: {
  //         actor: atomicassets.accountName,
  //         permission: "eosio.code",
  //       },
  //       weight: 1,
  //     },
  //   ],
  // });
 
 
  
 
  
// Import the atomicassets contract account


// Call the get_schemas action




describe("create tool", () => {
  it("creates a tool with the provided attributes", async () => {
    // Arrange
    const contractAccount = "nazakat12345";
    const assetId = 1099557743634;
    const owner = player1.accountName;

    // Set the context to simulate the authenticated account
    tester.context = {
      actor: owner,
      permission: "active",
    };

    // Mock the required table data
    const assetsTableMock = {
      find: jest.fn().mockReturnValue({
        collection_name: "collection_name",
        schema_name: { value: "schema_name" },
        template_id: "template_id",
      }),
    };
    const schemasTableMock = {
      require_find: jest.fn().mockReturnValue({ format: "schema_format" }),
    };
    const templatesTableMock = {
      require_find: jest.fn().mockReturnValue({
        immutable_serialized_data: "serialized_data",
      }),
    };
    const toolsTableMock = {
      emplace: jest.fn(),
    };

    // Mock the required functions from the smart contract
    contractMocks.push({
      receiver: "farmerworld",
      action: "get_assets",
      mock: jest.fn().mockReturnValue(assetsTableMock),
    });
    contractMocks.push({
      receiver: "farmerworld",
      action: "get_schemas",
      mock: jest.fn().mockReturnValue(schemasTableMock),
    });
    contractMocks.push({
      receiver: "farmerworld",
      action: "get_templates",
      mock: jest.fn().mockReturnValue(templatesTableMock),
    });
    contractMocks.push({
      receiver: "farmerworld",
      action: "tool_table",
      mock: jest.fn().mockReturnValue(toolsTableMock),
    });

    // Act
    await tester.contract.createtool(
      { contract_account: contractAccount, asset_id: assetId, owner },
      [{ actor: owner, permission: "active" }]
    );

    // Assert
    expect(toolsTableMock.emplace).toHaveBeenCalledWith(owner, expect.any(Function));

    // You can add more assertions to verify the tool attributes if necessary
    const [emplaceOwner, emplaceTool] = toolsTableMock.emplace.mock.calls[0];
    expect(emplaceOwner).toBe(owner);
    expect(emplaceTool.tool_id).toBe(assetId);
    expect(emplaceTool.owner).toBe(owner);
    // Assert the other tool attributes as needed
  });
});





  it("can add a player", async () => {
    expect.assertions(1);
    
    // Perform the addplayer action
    await tester.contract.addplayer({
      account: player1.accountName,
      energy: 10,
      wood: 10,
      food: 10,
      gold: 10
    }, [{ actor: player1.accountName, permission: "active" }]
    );

    expect(tester.getTableRowsScoped(`players`)['farmerworld']).toEqual([{
      account: player1.accountName,
      energy: 10,
      wood: 10,
      food: 10,
      gold: 10,
    },
    ]);
  });

  
  it("can add energy to a player", async () => {
    expect.assertions(2);
  
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
  
    // Find the updated player based on the account name
    const updatedPlayer = updatedPlayers.find(player => player.account === player1.accountName);
    console.log('updatePlayer', player1.accountName)
    // Calculate the expected energy value
    const expectedEnergy =  10 + 5 * 5;
    const expectedFood = 10 - 5 ; 
    // updatedPlayer.energy = updatedPlayer.energy - 10 ;
    expect(updatedPlayer.energy).toEqual(expectedEnergy);
    expect(updatedPlayer.food).toEqual(expectedFood);
  });
  



  // it.only("can create a tool", async () => {
  //   // Perform the createtool action
  //   console.log('log', player2.contract.mintasset);
  //   // ... (other properties)
    
  //   // Use the schema template as needed in your test case
  //   // For example, you can use it to deserialize data using the format field
  //   await tester.contract.createtool({
  //     contract_account: 'nazakat12345',
  //     asset_id: 1099557743634,
  //     owner: player1.accountName,
  //   }, [{ actor: player1.accountName, permission: "active" }]);
  
  //   // Check if the tool was created
  //   const tools = tester.getTableRowsScoped("toolslist")['farmerworld'];
  
  //   expect(tools).toEqual([
  //     {
  //       tool_id: assetId,
  //       owner: owner
        
  //     },
  //   ]);
  // });
  



 

  // it("can wear a tool", async () => {
  //   expect.assertions(1);

  //   await tester.contract.weartool(player1.accountName, 1);

  //   const tools = await tester.getTableRowsScoped("farmerworld", tester.accountName, "toolslist");
  //   expect(tools).toEqual([
  //     {
  //       tool_id: 1,
  //       owner: player1.accountName,
  //       name: "",
  //       img: "",
  //       rarity: "",
  //       level: 0,
  //       type: "",
  //       is_wear: true,
  //       durability: 0,
  //       current_durability: 0,
  //       lastChargeTime: 0,
  //     },
  //   ]);
  // })

  afterAll(() => {
    blockchain.persist();
  });
})

 // it("can wear a tool", async () => {
  //   expect.assertions(1);

  //   await tester.contract.weartool(player1.accountName, 1);

  //   const tools = await tester.getTableRowsScoped("farmerworld", tester.accountName, "toolslist");
  //   expect(tools).toEqual([
  //     {
  //       tool_id: 1,
  //       owner: player1.accountName,
  //       name: "",
  //       img: "",
  //       rarity: "",
  //       level: 0,
  //       type: "",
  //       is_wear: true,
  //       durability: 0,
  //       current_durability: 0,
  //       lastChargeTime: 0,
  //     },
  //   ]);
  // })




 
  // it.only("create minimal template", async () => {
  //   const immutableData = [
  //     { key: "name", value: "axe" },
  //     { key: "img", value: "QmYaAQJpiZDWPZQbcj5ph5nJJXeXUxQt89dLEf4xkWU4rB" },
  //     { key: "rarity", value: "Rare" },
  //     { key: "level", value: "3" },
  //     { key: "type", value: "Wood" },
  //     { key: "durability", value: "150" }
  //   ];
  
  //   const createTemplateAction = {
  //     authorized_creator: player1.accountName,
  //     collection_name: "testcollect1",
  //     schema_name: "testschema",
  //     transferable: true,
  //     burnable: true,
  //     max_supply: 0,
  //     immutable_data: []
  //   };
  
  //   // Serialize immutable data
  //   immutableData.forEach(({ key, value }) => {
  //     createTemplateAction.immutable_data.push([key, value]);
  //   });
  
  //   try {
  //     await atomicassets.contract.createtempl(createTemplateAction, [
  //       { actor: player1.accountName, permission: "active" }
  //     ]);
  //   } catch (error) {
  //     console.error(error);
  //   }
  
  //   const collectionTemplates = atomicassets.getTableRowsScoped("templates")["testcollect1"];
  //   expect(collectionTemplates).toEqual([
  //     {
  //       template_id: 1,
  //       schema_name: "testschema",
  //       transferable: true,
  //       burnable: true,
  //       max_supply: 0,
  //       issued_supply: 0,
  //       immutable_data: immutableData
  //     }
  //   ]);
  // });




  ///test cases


   


 
  

 
  
 

 

  


  




 

 
