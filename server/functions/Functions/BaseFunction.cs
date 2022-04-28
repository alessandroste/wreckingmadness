namespace WreckingMadness.Functions
{
    using Microsoft.Azure.Cosmos;
    using WreckingMadness.Functions.Model;

    public abstract class BaseFunction
    {
        protected readonly Configuration configuration;

        protected readonly CosmosClient cosmosClient;

        protected BaseFunction(
            Configuration configuration,
            CosmosClient cosmosClient
        )
        {
            this.configuration = configuration;
            this.cosmosClient = cosmosClient;
        }
    }
}