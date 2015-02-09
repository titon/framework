<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class RegistryTest extends TestCase {

    public function testAll() {
        $base = new RegistryStub();
        $config = new Config();
        $registry = new Registry();

        Registry::set($base);
        Registry::set($config);
        Registry::set($registry);

        $this->assertEquals(new Map([
            'Titon\Utility\RegistryStub' => $base,
            'Titon\Utility\Config' => $config,
            'Titon\Utility\Registry' => $registry
        ]), Registry::all());
    }

    public function testFactory() {
        $this->assertInstanceOf('Titon\Utility\RegistryStub', Registry::factory('Titon\Utility\RegistryStub', [], false));
        $this->assertInstanceOf('Titon\Utility\RegistryStub', Registry::factory('Titon/Utility/RegistryStub', [], false));
        $this->assertInstanceOf('Titon\Utility\RegistryStub', Registry::factory('Titon\Utility\RegistryStub', [], false));
        $this->assertInstanceOf('Titon\Utility\RegistryStub', Registry::factory('/Titon/Utility/RegistryStub', [], false));
    }

    public function testFlushAndKeys() {
        $test = Vector {};

        for ($i = 1; $i <= 10; $i++) {
            Registry::set(new RegistryStub(), 'key' . $i);
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
            Registry::set(new RegistryStub(), 'key' . $i);
        }

        $this->assertTrue(Registry::has('key1'));
        $this->assertTrue(Registry::has('key4'));
        $this->assertTrue(Registry::has('key8'));
        $this->assertFalse(Registry::has('key20'));
        $this->assertFalse(Registry::has('key25'));
        $this->assertFalse(Registry::has('key28'));
    }

    /**
     * @expectedException \Titon\Utility\Exception\InvalidObjectException
     */
    public function testSetInvalidObject() {
        Registry::set(12345);
    }

    public function testRemove() {
        for ($i = 1; $i <= 10; $i++) {
            Registry::set(new RegistryStub(), 'key' . $i);
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
            $base = new RegistryStub();
            $base->key = 'registry';

            return $base;
        });

        $object = Registry::get('base');

        $this->assertInstanceOf('Titon\Utility\RegistryStub', $object);
        $this->assertEquals('registry', $object->key);

        $this->assertInstanceOf('Titon\Utility\RegistryStub', Registry::get('base'));
    }

    /**
     * @expectedException \Titon\Utility\Exception\MissingObjectException
     */
    public function testGetInvalidKey() {
        Registry::get('missingKey');
    }

}

class RegistryStub {
}
