<?hh
namespace Titon\Debug;

use Titon\Test\TestCase;

class BenchmarkTest extends TestCase {

    public function testBenchmarking() {
        Benchmark::start('test');

        $benchmark = Benchmark::get('test');

        $this->assertGreaterThan(0.0, $benchmark['time.start']);
        $this->assertGreaterThan(0, $benchmark['memory.start']);
        $this->assertEquals(0.0, $benchmark['time.stop']);
        $this->assertEquals(0, $benchmark['memory.stop']);
        $this->assertTrue($benchmark['running']);

        Benchmark::stop('test');

        $benchmark = Benchmark::get('test');

        $this->assertGreaterThan(0.0, $benchmark['time.stop']);
        $this->assertGreaterThan(0, $benchmark['memory.stop']);
        $this->assertFalse($benchmark['running']);
        $this->assertTrue(count(Benchmark::all()) == 1);
        $this->assertEquals(sprintf('[test] %s seconds, %s memory (%s peak)', number_format($benchmark['time.avg'], 4), $benchmark['memory.avg'], $benchmark['memory.peak']), Benchmark::output('test'));
    }

    /**
     * @expectedException \Titon\Debug\Exception\MissingBenchmarkException
     */
    public function testGetInvalidKey() {
        Benchmark::get('fake');
    }

}
