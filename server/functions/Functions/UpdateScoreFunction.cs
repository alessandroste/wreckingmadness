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

    public class UpdateScoreFunction : BaseFunction
    {
        public UpdateScoreFunction(
            Configuration configuration,
            CosmosClient cosmosClient)
            : base(configuration, cosmosClient)
        {
        }

        [FunctionName("UpdateScore")]
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
                var scoreToCompare = data.TopScore;
                logger.LogInformation("Received score information, trying creating");
                var created = await TryCreateItemAsync(container, data, cancellationToken);
                if (!created)
                {
                    logger.LogInformation("Creation failed, checking if update is needed");
                    var currentItem = await container.ReadItemAsync<Player>(data.Identifier.ToString(), PartitionKey.None, cancellationToken: cancellationToken);
                    if (!currentItem.Resource.TopScore.HasValue || currentItem.Resource.TopScore.Value < data.TopScore)
                    {
                        logger.LogInformation("Score needs to be updated");
                        await container.UpsertItemAsync(data, PartitionKey.None, cancellationToken: cancellationToken);
                    }
                    else
                    {
                        scoreToCompare = currentItem.Resource.TopScore;
                    }
                }

                logger.LogInformation("Requesting percentile for score {0}", scoreToCompare.Value);
                var percentile = await GetPercentileAsync(container, scoreToCompare.Value, this.configuration.CollectionName, cancellationToken);
                data.Percentile = percentile;
                logger.LogInformation("Retrieved percentile {0}", percentile);

                return new OkObjectResult(data);
            }
            catch (CosmosException cosmosException)
            {
                logger.LogError(cosmosException, "Cosmos request failed");
                return new BadRequestObjectResult("Failed to process request.");
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

        private static async Task<float> GetPercentileAsync(Container container, uint score, string collectionName, CancellationToken cancellationToken)
        {
            var queryDefinition = new QueryDefinition(@$"
                SELECT (SELECT COUNT(1) * 100
                FROM {collectionName} s
                WHERE s.topScore < {score})/
                (SELECT COUNT()
                FROM {collectionName} s) AS percentage
            ");

            using var iterator = container.GetItemQueryIterator<float>(queryDefinition);
            var result = await iterator.ReadNextAsync(cancellationToken);
            return result.Single();
        }
    }
}