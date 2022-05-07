namespace WreckingMadness.Functions
{
    using System;
    using System.Net;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.AspNetCore.Http;
    using Microsoft.AspNetCore.Mvc;
    using Microsoft.Azure.Cosmos;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Azure.WebJobs.Extensions.Http;
    using Microsoft.Extensions.Logging;
    using WreckingMadness.Functions.Model;

    public class GetPlayerIdFunction : BaseFunction
    {
        public GetPlayerIdFunction(
            Configuration configuration,
            CosmosClient cosmosClient)
            : base(configuration, cosmosClient)
        {
        }

        [FunctionName("GetPlayerId")]
        public async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Function, "get")] HttpRequest request,
            ILogger logger,
            CancellationToken cancellationToken)
        {
            var container = this.cosmosClient.GetContainer(this.configuration.DatabaseName, this.configuration.CollectionName);

            do
            {
                var newId = Guid.NewGuid();
                try
                {
                    await container.ReadItemAsync<Player>(newId.ToString(), PartitionKey.None, cancellationToken: cancellationToken);
                }
                catch (CosmosException cosmosException) when (cosmosException.StatusCode == HttpStatusCode.NotFound)
                {
                    return new OkObjectResult(newId);
                }
                catch (CosmosException cosmosException)
                {
                    logger.LogError(cosmosException, "Received exception from Cosmos");
                    return new StatusCodeResult((int)HttpStatusCode.ServiceUnavailable);
                }
            } while (!cancellationToken.IsCancellationRequested);

            return new BadRequestObjectResult("Cancelled");
        }
    }
}