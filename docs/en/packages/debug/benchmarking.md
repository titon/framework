# Benchmarking #

Benchmarking is the process of comparing performance -- execution time and memory usage -- from one point to another.
This functionality can be achieved using the static `Titon\Debug\Benchmark` class.

## Capturing Metrics ##

To start the benchmarking process, simply call `start()`. This will log the start time and current memory usage.

```hack
Titon\Debug\Benchmark::start('foo');
```

And to stop the process, call `stop()`. Once stopped, the end time and memory usage will be logged, 
and averages will be calculated.

```hack
Titon\Debug\Benchmark::stop('foo');
```

Both of these calls generate a `Titon\Debug\Metric` shape, which is a custom [type alias](types.md) for benchmarking.

## Understanding Metrics ##

Once a `Metric` has been captured, it can be accessed using `get()`. 
If the `Metric` is accessed before it has been stopped, most of the values will be 0s.

```hack
$metric = Titon\Debug\Benchmark::get('foo');
```

The `Metric` is a [Hack shape](http://docs.hhvm.com/manual/en/hack.shapes.php) with the following fields. 

* `time.start` - The micro time when the benchmark started.
* `time.stop` - The micro time when the benchmark stopped.
* `time.age` - The start to stop time average.
* `memory.start` - The memory usage when the benchmark started.
* `memory.stop` - The memory usage when the benchmark stopped.
* `memory.age` - The start to stop memory average.
* `memory.peak` - The peak memory usage.
* `running` - Whether the benchmark is still running.

To print the `Metric` in a human readable format, use `output()`.
 
 ```hack
 $output = Titon\Debug\Benchmark::output('foo'); // [foo] 1.2 seconds, 45217 memory (56200 peak)
 ```
