<?hh
namespace Titon\Common;

use Titon\Test\Stub\Common\MacroableStub;
use Titon\Test\TestCase;

class MacroableTest extends TestCase {

    public function testGetHasMacro(): void {
        $this->assertFalse(MacroableStub::hasMacro('upper'));
        $this->assertFalse(MacroableStub::hasMacro('lower'));

        $lower = function(...$args) { return strtolower($args[0]); };
        $upper = function(...$args) { return strtoupper($args[0]); };

        MacroableStub::macro('lower', /* HH_FIXME[4039]: variadic + strict */ $lower);

        $this->assertFalse(MacroableStub::hasMacro('upper'));
        $this->assertTrue(MacroableStub::hasMacro('lower'));

        MacroableStub::macro('upper', /* HH_FIXME[4039]: variadic + strict */ $upper);

        $this->assertEquals(Map {
            'lower' => $lower,
            'upper' => $upper
        }, MacroableStub::getMacros());

        $this->assertTrue(MacroableStub::hasMacro('upper'));
        $this->assertTrue(MacroableStub::hasMacro('lower'));
    }

    public function testMacroCallback(): void {
        MacroableStub::macro('caps', /* HH_FIXME[4039]: variadic + strict */ function(...$args) { return strtoupper($args[0]); });

        $this->assertEquals('FOOBAR', MacroableStub::caps('foObAr'));
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingMacroException
     */
    public function testMacroMissing(): void {
        MacroableStub::lowers('foObAr');
    }

}
