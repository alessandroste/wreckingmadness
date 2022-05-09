namespace WreckingMadness.Functions
{
    using System;
    using System.Text.Json;
    using Microsoft.Azure.Cosmos.Fluent;
    using Microsoft.Azure.Functions.Extensions.DependencyInjection;
    using Microsoft.Extensions.Configuration;
    using Microsoft.Extensions.DependencyInjection;
    using WreckingMadness.Functions.Common;
    using WreckingMadness.Functions.Model;

    public class Startup : FunctionsStartup
    {
        private static readonly IConfigurationRoot configuration = new ConfigurationBuilder()
            .SetBasePath(Environment.CurrentDirectory)
            .AddJsonFile("AppSettings.json", optional: true, reloadOnChange: true)
            .AddEnvironmentVariables()
            .Build();

        public override void Configure(IFunctionsHostBuilder builder)
        {
            var myConfiguration = new Configuration();
            ConfigurationBinder.Bind(configuration, myConfiguration);
            builder.Services.AddSingleton(myConfiguration);
            builder.Services.AddSingleton((s) =>
            {
                string endpoint = myConfiguration.EndPointUrl;
                if (string.IsNullOrEmpty(endpoint))
                {
                    throw new ArgumentException("Please specify a valid endpoint in the appSettings.json file or your Azure Functions Settings.");
                }

                string authKey = myConfiguration.AuthorizationKey;
                if (string.IsNullOrEmpty(authKey) || string.Equals(authKey, "Super secret key"))
                {
                    throw new ArgumentException("Please specify a valid AuthorizationKey in the appSettings.json file or your Azure Functions Settings.");
                }

                var cosmosClientBuilder = new CosmosClientBuilder(endpoint, authKey);
                cosmosClientBuilder.WithCustomSerializer(new SystemSerializer(new JsonSerializerOptions(JsonSerializerDefaults.Web)));
                return cosmosClientBuilder.Build();
            });
        }
    }
}