namespace WreckingMadness.Functions
{
    using System.Linq;
    using System.Net;
    using System.Text.Json;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.AspNetCore.Http;
    using Microsoft.AspNetCore.Mvc;
    using Microsoft.Azure.Cosmos;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Azure.WebJobs.Extensions.Http;
    using Microsoft.Extensions.Logging;
    using WreckingMadness.Functions.Model;

    public class UpdateTopScoreForPlayerFunction : BaseFunction
    {
        public UpdateTopScoreForPlayerFunction(
            Configuration configuration,
            CosmosClient cosmosClient)
            : base(configuration, cosmosClient)
        {
        }

        [FunctionName("UpdateTopScoreForPlayer")]
        public async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Function, "post")] HttpRequest request,
            ILogger logger,
            CancellationToken cancellationToken)
        {
            logger.LogInformation($"Updating score");

            var data = await JsonSerializer.DeserializeAsync<Player>(request.Body, cancellationToken: cancellationToken);
            if (data == null)
            {
                return new BadRequestObjectResult($"Cannot parse body.");
            }

            if (data.Identifier == default || data.TopScore == default)
            {
                return new BadRequestObjectResult($"Request is invalid");
            }

            var container = this.cosmosClient.GetContainer(this.configuration.DatabaseName, this.configuration.CollectionName);

            try
            {
                var percentile = await GetPercentileAsync(container, data, this.configuration.CollectionName, cancellationToken);
                data.Percentile = percentile;

                var created = await TryCreateItemAsync(container, data, cancellationToken);
                if (!created)
                {
                    await container.UpsertItemAsync(data, PartitionKey.None, cancellationToken: cancellationToken);
                }

                return new OkResult();
            }
            catch (CosmosException cosmosException)
            {
                logger.LogError("Creating item failed with error {0}", cosmosException.ToString());
                return new BadRequestObjectResult($"Failed to process request.");
            }
        }

        private static async Task<bool> TryCreateItemAsync(Container container, Player data, CancellationToken cancellationToken)
        {
            try
            {
                var result = await container.CreateItemAsync(data, PartitionKey.None, cancellationToken: cancellationToken);
                return true;
            }
            catch (CosmosException cosmosException) when (cosmosException.StatusCode == HttpStatusCode.Conflict)
            {
                return false;
            }
        }

        private static async Task<float> GetPercentileAsync(Container container, Player data, string collectionName, CancellationToken cancellationToken)
        {
            var queryDefinition = new QueryDefinition(@$"
                SELECT (SELECT COUNT(1) * 100
                FROM {collectionName} s
                WHERE s.topScore < {data.TopScore})/
                (SELECT COUNT()
                FROM {collectionName} s) AS percentage
            ");

            using var iterator = container.GetItemQueryIterator<float>(queryDefinition);
            var result = await iterator.ReadNextAsync(cancellationToken);
            return result.Single();
        }
    }
}