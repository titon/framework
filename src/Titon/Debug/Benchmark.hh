<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug;

use Titon\Debug\Exception\MissingBenchmarkException;

/**
 * Delivers the functionality to start, stop and log benchmarks.
 * Benchmarks store the time difference and memory usage between two blocks during runtime.
 *
 * @package Titon\Debug
 */
class Benchmark {

    /**
     * User and system initiated benchmarking tests.
     *
     * @var \Titon\Debug\MetricMap
     */
    protected static MetricMap $benchmarks = Map {};

    /**
     * Return all benchmarks and calculate averages.
     *
     * @return \Titon\Debug\MetricMap
     */
    public static function all(): MetricMap {
        return static::$benchmarks;
    }

    /**
     * Return a single benchmark metric by key.
     *
     * @param string $key
     * @return \Titon\Debug\Metric
     * @throws \Titon\Debug\Exception\MissingBenchmarkException
     */
    public static function get(string $key): Metric {
        if (!static::has($key)) {
            throw new MissingBenchmarkException(sprintf('Benchmark %s does not exist', $key));
        }

        return static::$benchmarks[$key];
    }

    /**
     * Return true if a metric exists by key.
     *
     * @param string $key
     * @return bool
     */
    public static function has(string $key): bool {
        return static::$benchmarks->contains($key);
    }

    /**
     * Outputs and formats a benchmark directly as a string.
     *
     * @param string $key
     * @return string
     */
    public static function output(string $key): string {
        $benchmark = static::get($key);

        return sprintf('[%s] %s seconds, %s memory (%s peak)',
            $key,
            number_format($benchmark['time.avg'], 4),
            $benchmark['memory.avg'],
            $benchmark['memory.peak']);
    }

    /**
     * Start the benchmarking process by logging the micro seconds and memory usage.
     *
     * @param string $key
     */
    public static function start(string $key): void {
        static::$benchmarks[$key] = shape(
            'time.start' => microtime(true),
            'time.stop' => 0.0,
            'time.avg' => 0.0,
            'memory.start' => memory_get_usage(true),
            'memory.stop' => 0,
            'memory.avg' => 0,
            'memory.peak' => 0,
            'running' => true
        );
    }

    /**
     * Stop the benchmarking process by logging the micro seconds, memory usage and calculating averages.
     *
     * @param string $key
     */
    public static function stop(string $key): void {
        $benchmark = static::get($key);
        $benchmark['time.stop'] = microtime(true);
        $benchmark['time.avg'] = $benchmark['time.stop'] - $benchmark['time.start'];
        $benchmark['memory.stop'] = memory_get_usage(true);
        $benchmark['memory.avg'] = $benchmark['memory.stop'] - $benchmark['memory.start'];
        $benchmark['memory.peak'] = memory_get_peak_usage();
        $benchmark['running'] = false;

        static::$benchmarks[$key] = $benchmark;
    }

}
