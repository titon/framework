<?hh
namespace Titon\Debug;

use Titon\Test\TestCase;

class BenchmarkTest extends TestCase {

    public function testBenchmarking() {
        Benchmark::start('test');

        $benchmark = Benchmark::get('test');

        $this->assertGreaterThan(0.0, $benchmark['startTime']);
        $this->assertGreaterThan(0, $benchmark['startMemory']);
        $this->assertEquals(0.0, $benchmark['endTime']);
        $this->assertEquals(0, $benchmark['endMemory']);
        $this->assertTrue($benchmark['running']);

        Benchmark::stop('test');

        $benchmark = Benchmark::get('test');

        $this->assertGreaterThan(0.0, $benchmark['endTime']);
        $this->assertGreaterThan(0, $benchmark['endMemory']);
        $this->assertFalse($benchmark['running']);
        $this->assertTrue(count(Benchmark::all()) == 1);
        $this->assertEquals(sprintf('[test] %s seconds, %s memory (%s peak)', number_format($benchmark['avgTime'], 4), $benchmark['avgMemory'], $benchmark['peakMemory']), Benchmark::output('test'));
    }

    /**
     * @expectedException \Titon\Debug\Exception\MissingBenchmarkException
     */
    public function testGetInvalidKey() {
        Benchmark::get('fake');
    }

}