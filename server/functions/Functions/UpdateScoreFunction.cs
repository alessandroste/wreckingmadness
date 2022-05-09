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
            var data = await JsonSerializer.DeserializeAsync<Player>(request.Body, new JsonSerializerOptions(JsonSerializerDefaults.Web), cancellationToken);
            if (data == null)
            {
                return new BadRequestObjectResult($"Cannot parse body.");
            }

            if (data.Id == default || data.TopScore == default)
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
                    var currentItem = await container.ReadItemAsync<Player>(data.Id.ToString(), new PartitionKey(data.Id.ToString()), cancellationToken: cancellationToken);
                    if (!currentItem.Resource.TopScore.HasValue || currentItem.Resource.TopScore.Value < data.TopScore)
                    {
                        logger.LogInformation("Score needs to be updated");
                        await container.UpsertItemAsync(data, cancellationToken: cancellationToken);
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
                var result = await container.CreateItemAsync(data, cancellationToken: cancellationToken);
                return true;
            }
            catch (CosmosException cosmosException) when (cosmosException.StatusCode == HttpStatusCode.Conflict)
            {
                return false;
            }
        }

        private static async Task<float> GetPercentileAsync(Container container, uint score, string collectionName, CancellationToken cancellationToken)
        {
            var lowerScoresQueryDefinition = new QueryDefinition(@$"
                SELECT VALUE COUNT(1)
                FROM {collectionName} s
                WHERE s.topScore < {score}
            ");
            var allScoresQueryDefiniton = new QueryDefinition(@$"
                SELECT VALUE COUNT(1)
                FROM {collectionName}
            ");

            using var partialScoresIterator = container.GetItemQueryIterator<float>(lowerScoresQueryDefinition);
            var partialScoresResult = await partialScoresIterator.ReadNextAsync(cancellationToken);
            using var totalScoresIterator = container.GetItemQueryIterator<float>(allScoresQueryDefiniton);
            var totalScoresResult = await totalScoresIterator.ReadNextAsync(cancellationToken);
            return partialScoresResult.Single() * 100.0f / totalScoresResult.Single();
        }
    }
}