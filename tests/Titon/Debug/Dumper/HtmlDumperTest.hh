<?hh // strict
namespace Titon\Debug\Dumper;

use Titon\Debug\Exception\FatalErrorException;
use Titon\Test\TestCase;

/**
 * @property \Titon\Debug\Dumper\HtmlDumper $object
 */
class HtmlDumperTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new HtmlDumper();
    }

    public function testBacktrace(): void {
        $this->assertRegExp('/^<div class="titon-backtrace">/', trim($this->object->backtrace()));
    }

    public function testDebug(): void {
        $this->assertRegExp('/^<div class="titon-debug">/', $this->object->debug(1));
    }

    public function testDump(): void {
        $this->assertRegExp('/^<div class="titon-dump">/', $this->object->dump(1));
    }

    public function testInspect(): void {
        $this->assertRegExp('/^<div class="titon-inspect">/', $this->object->inspect(new FatalErrorException('Systems critical!')));
    }

}
