<?php
namespace Titon\Utility;

use Titon\Test\TestCase;

class MacroTest extends TestCase {

    public function testHasMacro() {
        $this->assertFalse(Number::hasMacro('toBinary'));
        $this->assertFalse(Number::hasMacro('toFloat'));

        Number::macro('toFloat', function() {});

        $this->assertFalse(Number::hasMacro('toBinary'));
        $this->assertTrue(Number::hasMacro('toFloat'));
    }

    public function testHasMethod() {
        $this->assertTrue(Inflector::hasMethod('slug'));
        $this->assertFalse(Inflector::hasMethod('slugify'));

        Inflector::macro('slugify', function() {});

        $this->assertTrue(Inflector::hasMethod('slug'));
        $this->assertTrue(Inflector::hasMethod('slugify'));
    }

    public function testInheritance() {
        $this->assertFalse(Format::hasMethod('foobar'));
        $this->assertFalse(Path::hasMethod('foobar'));

        Format::macro('foobar', function() {});

        $this->assertTrue(Format::hasMethod('foobar'));
        $this->assertFalse(Path::hasMethod('foobar'));
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

    public function testMacros() {
        $lower = function($value) {
            return strtolower($value);
        };

        $upper = function($value) {
            return strtoupper($value);
        };

        String::macro('lower', $lower);
        String::macro('upper', $upper);

        $this->assertEquals(array(
            'lower' => $lower,
            'upper' => $upper
        ), String::macros());
    }

}