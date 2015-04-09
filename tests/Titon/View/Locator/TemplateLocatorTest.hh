<?hh
namespace Titon\View\Locator;

use Titon\Test\TestCase;
use Titon\View\Template;
use Titon\View\ViewTest;

/**
 * @property \Titon\View\Locator\TemplateLocator $object
 */
class AssetHelperTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure(ViewTest::generateViewStructure());

        $this->object = new TemplateLocator([
            $this->vfs()->path('/views'),
            $this->vfs()->path('/views/fallback')
        ]);
    }

    public function testLocate(): void {
        $this->assertEquals($this->vfs()->path('/views/public/index/add.tpl'), $this->object->locate('index/add'));
        $this->assertEquals($this->vfs()->path('/views/public/index/add.tpl'), $this->object->locate('index\add'));
        $this->assertEquals($this->vfs()->path('/views/public/index/add.tpl'), $this->object->locate('index/add.tpl'));
        $this->assertEquals($this->vfs()->path('/views/public/index/view.xml.tpl'), $this->object->locate('index/view.xml'));

        // partials
        $this->assertEquals($this->vfs()->path('/views/private/partials/include.tpl'), $this->object->locate('include', Template::PARTIAL));
        $this->assertEquals($this->vfs()->path('/views/private/partials/nested/include.tpl'), $this->object->locate('nested/include', Template::PARTIAL));

        // wrapper
        $this->assertEquals($this->vfs()->path('/views/private/wrappers/wrapper.tpl'), $this->object->locate('wrapper', Template::WRAPPER));
        $this->assertEquals($this->vfs()->path('/views/fallback/private/wrappers/fallback.tpl'), $this->object->locate('fallback', Template::WRAPPER));

        // layout
        $this->assertEquals($this->vfs()->path('/views/private/layouts/default.tpl'), $this->object->locate('default', Template::LAYOUT));
        $this->assertEquals($this->vfs()->path('/views/fallback/private/layouts/fallback.tpl'), $this->object->locate('fallback', Template::LAYOUT));

        // private
        $this->assertEquals($this->vfs()->path('/views/private/errors/404.tpl'), $this->object->locate('errors/404', Template::CLOSED));
        $this->assertEquals($this->vfs()->path('/views/fallback/private/emails/example.html.tpl'), $this->object->locate('emails/example.html', Template::CLOSED));
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTemplateException
     */
    public function testLocateMissing(): void {
        $this->object->locate('index/missing');
    }

}

