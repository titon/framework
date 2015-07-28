<?hh
namespace Titon\Route;

use Titon\Test\TestCase;

class LocaleRouteTest extends TestCase {

    public function testCompile(): void {
        $moduleControllerActionExt = new LocaleRoute('/{module}/{controller}/{action}.{ext}', 'Controller@action');
        $moduleControllerAction = new LocaleRoute('/{module}/{controller}/{action}', 'Controller@action');
        $moduleController = new LocaleRoute('/{module}/{controller}', 'Controller@action');
        $module = new LocaleRoute('/{module}', 'Controller@action');
        $root = new LocaleRoute('/', 'Controller@action');

        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\.([a-z0-9\_\-\.]+)\/?', $moduleControllerActionExt->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/?', $moduleControllerAction->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/([a-z0-9\_\-\.]+)\/([a-z0-9\_\-\.]+)\/?', $moduleController->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/([a-z0-9\_\-\.]+)\/?', $module->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/?', $root->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/?', $root->compile()); // Should return compiled again
    }

    public function testIsMatch(): void {
        $route = new LocaleRoute('/blog/[year]/[month]/[day]', 'Blog\Api@archives');

        $this->assertTrue($route->isMatch('/en-us/blog/2012/02/26'));
        $this->assertEquals(Map {
            'locale' => 'en-us',
            'year' => 2012,
            'month' => 2,
            'day' => 26
        }, $route->getParams());

        $route = new LocaleRoute('/{module}/{controller}/{action}/[id]', 'Module\Controller@action');

        $this->assertTrue($route->isMatch('/en/forum/topic/view/123'));
        $this->assertEquals(Map {
            'locale' => 'en',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'view',
            'id' => 123
        }, $route->getParams());

        $this->assertFalse($route->isMatch('/foo-bar/forum/topic/view/123'));
        $this->assertFalse($route->isMatch('/forum/topic/view/123'));
    }

}
