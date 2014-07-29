<?hh
namespace Titon\Http\Bag;

use Titon\Http\Http;
use Titon\Test\TestCase;
use Titon\Utility\Crypt;

/**
 * @property \Titon\Http\Bag\CookieBag $object
 */
class CookieBagTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new CookieBag($_COOKIE, Map {'encrypt' => false});
    }

    public function testEncryptDecrypt() {
        $this->object->setConfig('encrypt', Crypt::RIJNDAEL);

        $this->assertEquals('bar', $this->object->decrypt('foo', $this->object->encrypt('foo', 'bar')));
        $this->assertEquals(['bar'], $this->object->decrypt('foo', $this->object->encrypt('foo', ['bar'])));
    }

    public function testPrepare() {
        $this->object->setConfig('encrypt', Crypt::RIJNDAEL);

        $time = strtotime('+1 day');
        $expires = gmdate(Http::DATE_FORMAT, $time);

        $this->assertEquals('foo=WBQ%A3E%AA6%CD%E8%D9%CCC%E9%8D%01%8B; Expires=' . $expires . '; Path=/; HttpOnly', $this->object->prepare('foo', 'bar', Map {
            'expires' => $time
        }));

        $this->assertEquals('foo=WBQ%A3E%AA6%CD%E8%D9%CCC%E9%8D%01%8B; Expires=' . $expires, $this->object->prepare('foo', 'bar', Map {
            'expires' => $time,
            'path' => '',
            'httpOnly' => false
        }));

        $this->assertEquals('foo=WBQ%A3E%AA6%CD%E8%D9%CCC%E9%8D%01%8B; Expires=' . $expires . '; Domain=.example.com; HttpOnly', $this->object->prepare('foo', 'bar', Map {
            'expires' => $time,
            'path' => '',
            'domain' => '.example.com'
        }));

        $this->assertEquals('foo=WBQ%A3E%AA6%CD%E8%D9%CCC%E9%8D%01%8B; Expires=' . $expires . '; Path=/baz; Domain=.example.com; Secure; HttpOnly', $this->object->prepare('foo', 'bar', Map {
            'expires' => $time,
            'path' => '/baz',
            'domain' => '.example.com',
            'secure' => true
        }));

        $deleted = time();

        $this->assertEquals('foo=deleted; Expires=' . gmdate(Http::DATE_FORMAT, $deleted) . '; Path=/; HttpOnly', $this->object->prepare('foo', 'bar', Map {
            'expires' => $deleted
        }));
    }

    public function testAllWithEncryption() {
        $this->object->setConfig('encrypt', Crypt::RIJNDAEL);

        $this->object->set('key', 'value');
        $this->assertTrue($this->object->has('key'));
        $this->assertEquals('value', $this->object->get('key'));

        // getting then setting then getting
        $this->assertFalse($this->object->has('foo'));
        $this->assertEquals(null, $this->object->get('foo'));

        $this->object->set('foo', 'bar');
        $this->assertEquals('bar', $this->object->get('foo'));

        $this->object->set('foo', 'baz');
        $this->assertEquals('baz', $this->object->get('foo'));

        // removing a value
        $this->object->remove('foo');
        $this->assertEquals(null, $this->object->get('foo'));
    }

    public function testAllWithNoEncryption() {
        $this->object->set('key', 'value');
        $this->assertEquals('value', $this->object->get('key'));

        // getting then setting then getting
        $this->object->set('foo', 'bar');
        $this->assertEquals('bar', $this->object->get('foo'));

        // removing a value
        $this->object->remove('foo');
        $this->assertEquals(null, $this->object->get('foo'));
    }

}