<?php
namespace Titon\Http\Server;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Server\Session $object
 * @property \Titon\Http\Server\Request $request
 *
 * @runTestsInSeparateProcesses
 */
class SessionTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $_SERVER['HTTP_USER_AGENT'] = 'titon';

        $this->object = new Session([
            'initialize' => true,
            'lifetime' => 360,
            'ini' => [
                'session.cookie_secure' => true
            ]
        ]);

        $this->request = Request::createFromGlobals();
    }

    public function testInitialize() {
        if (defined('HHVM_VERSION')) {
            $this->markTestSkipped('HHVM does not allow setting of ini for sessions');
        }

        $this->object->startup($this->request);

        $this->assertEquals('titon', ini_get('session.name'));
        $this->assertEquals(360, ini_get('session.cookie_lifetime'));
        $this->assertEquals(true, ini_get('session.cookie_secure'));
        $this->assertEquals('localhost', ini_get('session.cookie_domain'));
        $this->assertEquals('localhost', ini_get('session.referer_check'));

        // Check security was set
        $this->assertTrue($this->object->has('titon'));
    }

    public function testGlobalReference() {
        $this->object->startup($this->request);

        $this->object->set('key', 'value');
        $this->assertEquals('value', $this->object->get('key'));
        $this->assertEquals('value', $_SESSION['key']);

        $_SESSION['foo'] = 'bar';
        $this->assertEquals('bar', $this->object->get('foo'));
    }

    public function testGetSet() {
        $this->object->startup($this->request);

        $this->object->set('key', 'value');
        $this->assertEquals('value', $this->object->get('key'));

        $this->object->set('foo', 'bar');
        $this->assertEquals('bar', $this->object->get('foo'));

        $this->object->set('boolean', true);
        $this->assertEquals(true, $this->object->get('boolean'));

        $this->object->set('integer', 12345);
        $this->assertEquals(12345, $this->object->get('integer'));

        // fake key
        $this->assertEquals(null, $this->object->get('fakeKey'));

        // overwrite
        $this->object->set('boolean', false);
        $this->assertEquals(false, $this->object->get('boolean'));

        // nested
        $this->object->set('array.key', 'value');
        $this->assertEquals(['key' => 'value'], $this->object->get('array'));

        // get all
        $data = $this->object->all();
        unset($data['titon']);

        $this->assertEquals([
            'key' => 'value',
            'foo' => 'bar',
            'boolean' => false,
            'integer' => 12345,
            'array' => ['key' => 'value']
        ], $data);
    }

    public function testHas() {
        $this->object->startup($this->request);

        $this->assertFalse($this->object->has('key'));
        $this->assertFalse($this->object->has('foo.bar'));

        $this->object->set('key', 'value')->set('foo.bar', true);

        $this->assertTrue($this->object->has('key'));
        $this->assertTrue($this->object->has('foo.bar'));
    }

    public function testRemove() {
        $this->object->startup($this->request);

        $this->object->set('key', 'value')->set('foo.bar', true);

        $this->assertTrue($this->object->has('key'));
        $this->assertTrue($this->object->has('foo.bar'));

        $this->object->remove('foo.bar');

        $this->assertTrue($this->object->has('foo'));
        $this->assertFalse($this->object->has('foo.bar'));

        $this->object->remove('foo');

        $this->assertFalse($this->object->has('foo'));
        $this->assertFalse($this->object->has('foo.bar'));
    }

    public function testDestroy() {
        $this->object->startup($this->request);

        $this->object->set('key', 'value');
        $this->assertTrue($this->object->has('key'));

        $this->object->destroy();
        $this->assertFalse($this->object->has('key'));
        $this->assertEquals([], $this->object->all());
        $this->assertEquals([], $_SESSION);

        // set more again
        $this->object->set('key', 'value');
        $this->assertTrue($this->object->has('key'));
        $this->assertEquals('value', $_SESSION['key']);
    }

    public function testValidateAndPersist() {
        $this->object->startup($this->request);

        $token = $this->object->get('titon');

        // sleep so that we can cause time changes
        sleep(1);

        // changing browser should cause a destroy
        $this->object->getRequest()->setHeader('User-Agent', 'browser changed, hacking attempt!');
        $this->object->validate();

        $this->assertNotEquals($token['time'], $this->object->get('titon.time'));
        $this->assertNotEquals($token['agent'], $this->object->get('titon.agent'));

        // try again with no agent check
        $this->object->setConfig('checkUserAgent', false);
        $token = $this->object->get('titon');

        sleep(1);

        $this->object->getRequest()->setHeader('User-Agent', 'browser changed again, hacking attempt!');
        $this->object->validate();

        // should be equal since we dont care if user agent changed
        $this->assertEquals($token['time'], $this->object->get('titon.time'));
        $this->assertEquals($token['agent'], $this->object->get('titon.agent'));
    }

    public function testMultiSessionInstances() {
        $this->object->startup($this->request);

        $session = new Session();
        $session->startup($this->request);
        $session->foo = 'bar';

        $this->assertEquals('bar', $session->foo);
        $this->assertEquals('bar', $this->object->foo);
        $this->assertEquals('bar', $_SESSION['foo']);
    }

}