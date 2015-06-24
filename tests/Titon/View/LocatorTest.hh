<?hh
namespace Titon\View;

use Titon\Test\TestCase;
use Titon\Test\Stub\View\LocatorStub;
use Titon\Utility\Config;

/**
 * @property \Titon\View\Locator $object
 */
class LocatorTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new LocatorStub([
            '/views',
            '/views/fallback/',
            '/views/misc'
        ]);
    }

    public function testAddGetSetPaths(): void {
        $expected = Vector {
            '/views/',
            '/views/fallback/',
            '/views/misc/'
        };
        $this->assertEquals($expected, $this->object->getPaths());

        $this->object->addPath('/templates');

        $expected[] = '/templates/';
        $this->assertEquals($expected, $this->object->getPaths());

        $this->object->addPaths(Vector {
            '/templates/fallback',
            '/templates/misc'
        });

        $expected[] = '/templates/fallback/';
        $expected[] = '/templates/misc/';
        $this->assertEquals($expected, $this->object->getPaths());

        $this->object->setPaths(Vector {'/views'});

        $this->assertEquals(Vector {'/views/'}, $this->object->getPaths());
    }

    public function testPathFormatsThroughConstructor(): void {
        $object = new LocatorStub('/views');

        $this->assertEquals(Vector {'/views/'}, $object->getPaths());

        $object = new LocatorStub(['/views']);

        $this->assertEquals(Vector {'/views/'}, $object->getPaths());

        $object = new LocatorStub(Vector {'/views'});

        $this->assertEquals(Vector {'/views/'}, $object->getPaths());
    }

    public function testConfigPathsAreLoaded(): void {
        Config::set('titon.paths.views', Vector {'/views/inherited/'});

        $object = new LocatorStub('/views');

        $this->assertEquals(Vector {
            '/views/',
            '/views/inherited/'
        }, $object->getPaths());
    }

    public function testGetSetExtension(): void {
        $this->assertEquals('tpl', $this->object->getExtension());

        $this->object->setExtension('html');

        $this->assertEquals('html', $this->object->getExtension());

        $this->object->setExtension('.html'); // Strip period

        $this->assertEquals('html', $this->object->getExtension());
    }

    public function testExtensionIsSetThroughConstructor() : void {
        $object = new LocatorStub('/', 'html');

        $this->assertEquals('html', $object->getExtension());
    }

}
