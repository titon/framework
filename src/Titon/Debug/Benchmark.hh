<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Debug;

use Titon\Debug\Exception\MissingBenchmarkException;

type Metric = shape('running' => bool, 'startTime' => float, 'endTime' => float, 'avgTime' => float, 'startMemory' => int, 'endMemory' => int, 'avgMemory' => int, 'peakMemory' => int);
type MetricMap = Map<string, Metric>;

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
     * @type \Titon\Debug\MetricMap
     */
    protected static MetricMap $_benchmarks = Map {};

    /**
     * Return all benchmarks and calculate averages.
     *
     * @return \Titon\Debug\MetricMap
     */
    public static function all(): MetricMap {
        return static::$_benchmarks;
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

        return static::$_benchmarks[$key];
    }

    /**
     * Return true if a metric exists by key.
     *
     * @param string $key
     * @return bool
     */
    public static function has(string $key): bool {
        return static::$_benchmarks->contains($key);
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
            number_format($benchmark['avgTime'], 4),
            $benchmark['avgMemory'],
            $benchmark['peakMemory']);
    }

    /**
     * Start the benchmarking process by logging the micro seconds and memory usage.
     *
     * @param string $key
     */
    public static function start(string $key): void {
        static::$_benchmarks[$key] = shape(
            'startTime' => microtime(true),
            'endTime' => 0.0,
            'avgTime' => 0.0,
            'startMemory' => memory_get_usage(true),
            'endMemory' => 0,
            'avgMemory' => 0,
            'peakMemory' => 0,
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
        $benchmark['endTime'] = microtime(true);
        $benchmark['avgTime'] = $benchmark['endTime'] - $benchmark['startTime'];
        $benchmark['endMemory'] = memory_get_usage(true);
        $benchmark['avgMemory'] = $benchmark['endMemory'] - $benchmark['startMemory'];
        $benchmark['peakMemory'] = memory_get_peak_usage();
        $benchmark['running'] = false;

        static::$_benchmarks[$key] = $benchmark;
    }

}
