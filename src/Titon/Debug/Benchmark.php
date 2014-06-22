<?php
/**
 * @copyright   2010-2013, The Titon Project
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
     * @type array
     */
    protected static $_benchmarks = [];

    /**
     * Return all benchmarks and calculate averages.
     *
     * @return array
     */
    public static function all() {
        $benchmarks = static::$_benchmarks;

        foreach ($benchmarks as $key => $bm) {
            $benchmarks[$key] = static::get($key);
        }

        return $benchmarks;
    }

    /**
     * Return a single benchmark by key and calculate averages and memory usage.
     *
     * @param string $key
     * @return array
     * @throws \Titon\Debug\Exception\MissingBenchmarkException
     */
    public static function get($key) {
        if (empty(static::$_benchmarks[$key])) {
            throw new MissingBenchmarkException(sprintf('Benchmark %s does not exist', $key));
        }

        $bm = static::$_benchmarks[$key];
        $bm['avgTime'] = isset($bm['endTime']) ? ($bm['endTime'] - $bm['startTime']) : null;
        $bm['avgMemory'] = isset($bm['endMemory']) ? ($bm['endMemory'] - $bm['startMemory']) : null;
        $bm['peakMemory'] = memory_get_peak_usage();

        return $bm;
    }

    /**
     * Outputs and formats a benchmark directly as a string.
     *
     * @param string $key
     * @return string
     */
    public static function output($key) {
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
    public static function start($key) {
        static::$_benchmarks[$key] = [
            'startTime' => microtime(true),
            'startMemory' => memory_get_usage(true),
        ];
    }

    /**
     * Stop the benchmarking process by logging the micro seconds and memory usage.
     *
     * @param string $key
     */
    public static function stop($key) {
        static::$_benchmarks[$key] += [
            'endTime' => microtime(true),
            'endMemory' => memory_get_usage(true)
        ];
    }

}
