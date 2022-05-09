namespace WreckingMadness.Functions.Model
{
    using System;

    public class Player
    {
        public Guid? Id { get; set; }

        public uint? TopScore { get; set; }

        public float? Percentile { get; set; }
    }
}