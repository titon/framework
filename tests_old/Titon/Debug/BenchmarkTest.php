<?php
namespace Titon\Debug;

use Titon\Test\TestCase;

class BenchmarkTest extends TestCase {

    public function testBenchmarking() {
        Benchmark::start('test');
        $this->assertArrayHasKey('startTime', Benchmark::get('test'));
        $this->assertArrayHasKey('startMemory', Benchmark::get('test'));
        $this->assertArrayNotHasKey('endTime', Benchmark::get('test'));
        $this->assertArrayNotHasKey('endMemory', Benchmark::get('test'));

        Benchmark::stop('test');
        $this->assertArrayHasKey('endTime', Benchmark::get('test'));
        $this->assertArrayHasKey('endMemory', Benchmark::get('test'));

        $this->assertTrue(is_array(Benchmark::get('test')));
        $this->assertTrue(count(Benchmark::all()) == 1);

        // output
        $test = Benchmark::get('test');
        $this->assertEquals(sprintf('[test] %s seconds, %s memory (%s peak)', number_format($test['avgTime'], 4), $test['avgMemory'], $test['peakMemory']), Benchmark::output('test'));
    }

    /**
     * @expectedException \Titon\Debug\Exception\MissingBenchmarkException
     */
    public function testGetInvalidKey() {
        Benchmark::get('fake');
    }

}