<?hh
namespace Titon\Debug;

use Titon\Test\TestCase;

class BenchmarkTest extends TestCase {

    public function testBenchmarking() {
        Benchmark::start('test');
        $this->assertTrue(isset(Benchmark::get('test')['startTime']));
        $this->assertTrue(isset(Benchmark::get('test')['startMemory']));
        $this->assertFalse(isset(Benchmark::get('test')['endTime']));
        $this->assertFalse(isset(Benchmark::get('test')['endMemory']));

        Benchmark::stop('test');
        $this->assertTrue(isset(Benchmark::get('test')['endTime']));
        $this->assertTrue(isset(Benchmark::get('test')['endMemory']));

        $this->assertInstanceOf('HH\Map', Benchmark::get('test'));
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