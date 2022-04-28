namespace WreckingMadness.Functions.Model
{
    using System;
    using System.Text.Json.Serialization;

    public class Player
    {
        [JsonPropertyName("Id")]
        public Guid? Identifier { get; set; }

        public uint? TopScore { get; set; }

        public float? Percentile { get; set; }
    }
}