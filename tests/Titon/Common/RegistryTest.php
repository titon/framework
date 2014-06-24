<?php
namespace Titon\Common;

use Titon\Test\TestCase;

class RegistryTest extends TestCase {

    public function testAll() {
        $base = new Base();
        $config = new Config();
        $registry = new Registry();

        Registry::set($base);
        Registry::set($config);
        Registry::set($registry);

        $this->assertEquals([
            'Titon\Common\Base' => $base,
            'Titon\Common\Config' => $config,
            'Titon\Common\Registry' => $registry
        ], Registry::all());
    }

    public function testFactory() {
        $this->assertInstanceOf('Titon\Common\Base', Registry::factory('Titon\Common\Base', [], false));
        $this->assertInstanceOf('Titon\Common\Base', Registry::factory('Titon/Common/Base', [], false));
        $this->assertInstanceOf('Titon\Common\Base', Registry::factory('Titon\Common\Base', [], false));
        $this->assertInstanceOf('Titon\Common\Base', Registry::factory('/Titon/Common/Base', [], false));
    }

    public function testFlushAndKeys() {
        $test = [];

        for ($i = 1; $i <= 10; $i++) {
            Registry::set(new Base(), 'key' . $i);
            $test[] = 'key' . $i;
        }

        $registered = Registry::keys();

        $this->assertEquals($test, $registered);
        $this->assertEquals(10, count($registered));

        Registry::flush();

        $registered = Registry::keys();

        $this->assertEquals(0, count($registered));
    }

    public function testHasAndSet() {
        for ($i = 1; $i <= 10; $i++) {
            Registry::set(new Base(), 'key' . $i);
        }

        $this->assertTrue(Registry::has('key1'));
        $this->assertTrue(Registry::has('key4'));
        $this->assertTrue(Registry::has('key8'));
        $this->assertFalse(Registry::has('key20'));
        $this->assertFalse(Registry::has('key25'));
        $this->assertFalse(Registry::has('key28'));
    }

    /**
     * @expectedException \Titon\Common\Exception\InvalidObjectException
     */
    public function testSetInvalidObject() {
        Registry::set(12345);
    }

    public function testRemove() {
        for ($i = 1; $i <= 10; $i++) {
            Registry::set(new Base(), 'key' . $i);
        }

        $this->assertTrue(Registry::has('key1'));
        $this->assertTrue(Registry::has('key4'));
        $this->assertTrue(Registry::has('key8'));

        Registry::remove('key1');
        Registry::remove('key4');
        Registry::remove('key8');

        $this->assertFalse(Registry::has('key1'));
        $this->assertFalse(Registry::has('key4'));
        $this->assertFalse(Registry::has('key8'));
    }

    public function testRegisterAndGet() {
        Registry::register('base', function() {
            return new Base(['key' => 'registry']);
        });

        $object = Registry::get('base');

        $this->assertInstanceOf('Titon\Common\Base', $object);
        $this->assertEquals('registry', $object->getConfig('key'));

        $this->assertInstanceOf('Titon\Common\Base', Registry::get('base'));
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingObjectException
     */
    public function testGetInvalidKey() {
        Registry::get('missingKey');
    }

}
