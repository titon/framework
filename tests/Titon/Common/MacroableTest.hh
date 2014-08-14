<?hh
namespace Titon\Common;

use Titon\Test\TestCase;
use Titon\Utility\Crypt;
use Titon\Utility\Format;
use Titon\Utility\Inflector;
use Titon\Utility\Number;
use Titon\Utility\Path;

class MacroableTest extends TestCase {

    public function testHasMacro() {
        $this->assertFalse(Number::hasMacro('toBinary'));
        $this->assertFalse(Number::hasMacro('toFloat'));

        Number::macro('toFloat', function() {});

        $this->assertFalse(Number::hasMacro('toBinary'));
        $this->assertTrue(Number::hasMacro('toFloat'));
    }

    public function testInheritance() {
        $this->assertFalse(Format::hasMacro('foobar'));
        $this->assertFalse(Path::hasMacro('foobar'));

        Format::macro('foobar', function() {});

        $this->assertTrue(Format::hasMacro('foobar'));
        $this->assertFalse(Path::hasMacro('foobar'));
    }

    public function testMacro() {
        Inflector::macro('caps', function($value) {
            return strtoupper($value);
        });

        $this->assertEquals('FOOBAR', Inflector::caps('foObAr'));
    }

    /**
     * @expectedException \Titon\Utility\Exception\UnsupportedMethodException
     */
    public function testMacroMissing() {
        Inflector::lowers('foObAr');
    }

    public function testGetMacros() {
        $lower = function($value) {
            return strtolower($value);
        };

        $upper = function($value) {
            return strtoupper($value);
        };

        Crypt::macro('lower', $lower);
        Crypt::macro('upper', $upper);

        $this->assertEquals(Map {
            'lower' => $lower,
            'upper' => $upper
        }, Crypt::getMacros());
    }

}