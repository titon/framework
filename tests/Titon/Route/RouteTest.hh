<?hh
namespace Titon\Route;

use Titon\Test\TestCase;

class RouteTest extends TestCase {

    public function testAppendPrepend() {
        $route = new Route('', 'Controller@action');

        $this->assertEquals('/', $route->getPath());

        $route->prepend('/prepend/');
        $this->assertEquals('/prepend', $route->getPath());

        $route->prepend('before');
        $this->assertEquals('/before/prepend', $route->getPath());

        $route->prepend('{prefix}//');
        $this->assertEquals('/{prefix}/before/prepend', $route->getPath());

        $route->append('/append/');
        $this->assertEquals('/{prefix}/before/prepend/append', $route->getPath());

        $route->append('after//');
        $this->assertEquals('/{prefix}/before/prepend/append/after', $route->getPath());

        $route->append('//{suffix}');
        $this->assertEquals('/{prefix}/before/prepend/append/after/{suffix}', $route->getPath());
    }

    public function testCompile() {
        $moduleControllerActionExt = new Route('/{module}/{controller}/{action}.{ext}', 'Controller@action');
        $moduleControllerAction = new Route('/{module}/{controller}/{action}', 'Controller@action');
        $moduleController = new Route('/{module}/{controller}', 'Controller@action');
        $module = new Route('/{module}', 'Controller@action');
        $root = (new Route('/', 'Controller@action'))->setStatic(true);

        $this->assertEquals('\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\.([a-z\_\-\+]+)\/?', $moduleControllerActionExt->compile());
        $this->assertEquals('\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/?', $moduleControllerAction->compile());
        $this->assertEquals('\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/?', $moduleController->compile());
        $this->assertEquals('\/([a-z\_\-\+]+)\/?', $module->compile());
        $this->assertEquals('\/', $root->compile());

        $multi = new Route('{alpha}/[numeric]/(wildcard)/', 'Controller@action');

        $patterns = (new Route('<alnum>/<locale>', 'Controller@action'))->setPatterns(Map {
            'alnum' => Route::ALNUM,
            'locale' => '([a-z]{2}(?:-[a-z]{2})?)'
        });

        $allTypes = (new Route('/<locale>/{alpha}/(wildcard)/[numeric]/{alnum}', 'Controller@action'))->setPatterns(Map {
            'alnum' => Route::ALNUM,
            'locale' => '([a-z]{2}(?:-[a-z]{2})?)'
        });

        $this->assertEquals('\/([a-z\_\-\+]+)\/([0-9\.]+)\/(.*)\/?', $multi->compile());
        $this->assertEquals('\/([a-z0-9\_\-\+]+)\/([a-z]{2}(?:-[a-z]{2})?)\/?', $patterns->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/([a-z\_\-\+]+)\/(.*)\/([0-9\.]+)\/([a-z\_\-\+]+)\/?', $allTypes->compile());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingPatternException
     */
    public function testCompileMissingPattern() {
        $noPattern = new Route('<missing>', 'Controller@action');
        $noPattern->compile();
    }

    public function testCompileOptionalToken() {
        $route = new Route('/users/[id]', 'Controller@action');

        $this->assertEquals('\/users\/([0-9\.]+)\/?', $route->compile());
        $this->assertTrue($route->isMatch('/users/1'));
        $this->assertFalse($route->isMatch('/users'));

        $route = new Route('/users/[id?]', 'Controller@action');

        $this->assertEquals('\/users(?:\/([0-9\.]+))?\/?', $route->compile());
        $this->assertTrue($route->isMatch('/users/1'));
        $this->assertTrue($route->isMatch('/users/'));
        $this->assertTrue($route->isMatch('/users'));
    }

    public function testFilters() {
        $this->markTestIncomplete('@todo');
    }

    public function testIsMethod() {
        $noMethod = new Route('/', 'Controller@action');

        $singleMethod = (new Route('/', 'Controller@action'))->addMethod('POST');

        $multiMethod = (new Route('/', 'Controller@action'))->setMethods(Vector {'post', 'put'});

        $_SERVER['REQUEST_METHOD'] = 'GET';

        $this->assertTrue($noMethod->isMethod());
        $this->assertFalse($singleMethod->isMethod());
        $this->assertFalse($multiMethod->isMethod());

        $_SERVER['REQUEST_METHOD'] = 'POST';

        $this->assertTrue($noMethod->isMethod());
        $this->assertTrue($singleMethod->isMethod());
        $this->assertTrue($multiMethod->isMethod());

        $_SERVER['REQUEST_METHOD'] = 'PUT';

        $this->assertTrue($noMethod->isMethod());
        $this->assertFalse($singleMethod->isMethod());
        $this->assertTrue($multiMethod->isMethod());
    }

    public function testIsSecure() {
        $unsecureRoute = (new Route('/', 'Controller@action'))->setSecure(false);
        $secureRoute = (new Route('/', 'Controller@action'))->setSecure(true);

        $this->assertTrue($unsecureRoute->isSecure());
        $this->assertFalse($secureRoute->isSecure());

        $_SERVER['HTTPS'] = 'on';

        $this->assertTrue($unsecureRoute->isSecure());
        $this->assertTrue($secureRoute->isSecure());
    }

    public function testIsStatic() {
        $route = (new Route('/', 'Controller@action'))->setStatic(false);
        $staticRoute = (new Route('/', 'Controller@action'))->setStatic(true);
        $tokenRoute = new Route('/{module}', 'Controller@action');

        $this->assertFalse($route->isStatic());
        $this->assertTrue($staticRoute->isStatic());
        $this->assertFalse($tokenRoute->isStatic());
    }

    public function testIsMatch() {
        $this->markTestIncomplete('@todo');
    }

    public function testIsMatchSecure() {
        $this->markTestIncomplete('@todo');
    }

    public function testIsMatchDirectEqualPath() {
        $this->markTestIncomplete('@todo');
    }

    public function testIsMatchMethod() {
        $this->markTestIncomplete('@todo');
    }

    public function testIsMatchTrailingSlash() {
        $route = new Route('/{module}', 'Controller@action');

        $this->assertTrue($route->isMatch('/users'));
        $this->assertTrue($route->isMatch('/users/'));
        $this->assertFalse($route->isMatch('/users//'));
    }

    public function testMethods() {
        $this->markTestIncomplete('@todo');
    }

    public function testParams() {
        $this->markTestIncomplete('@todo');
    }

    public function testPatterns() {
        $this->markTestIncomplete('@todo');
    }

    public function testTokens() {
        $route = new Route('/{string}', 'Controller@action');
        $this->assertEquals('\/([a-z\_\-\+]+)\/?', $route->compile());

        $route = new Route('/{string?}', 'Controller@action');
        $this->assertEquals('(?:\/([a-z\_\-\+]+))?\/?', $route->compile());

        $route = new Route('/[int]', 'Controller@action');
        $this->assertEquals('\/([0-9\.]+)\/?', $route->compile());

        $route = new Route('/[int?]', 'Controller@action');
        $this->assertEquals('(?:\/([0-9\.]+))?\/?', $route->compile());

        $route = new Route('/(wildcard)', 'Controller@action');
        $this->assertEquals('\/(.*)\/?', $route->compile());

        $route = new Route('/(wildcard?)', 'Controller@action');
        $this->assertEquals('(?:\/(.*))?\/?', $route->compile());

        $route = (new Route('/<regex>', 'Controller@action'))->setPattern('regex', '[foo|bar]');
        $this->assertEquals('\/([foo|bar])\/?', $route->compile());

        $route = (new Route('/<regex?>', 'Controller@action'))->setPattern('regex', '[foo|bar]');
        $this->assertEquals('(?:\/([foo|bar]))?\/?', $route->compile());

        $route = new Route('/<regex:[foo|bar]>', 'Controller@action');
        $this->assertEquals('\/([foo|bar])\/?', $route->compile());

        $route = new Route('/<regex:[foo|bar]?>', 'Controller@action');
        $this->assertEquals('(?:\/([foo|bar]))?\/?', 'Controller@action', $route->compile());

        $route = new Route('/<regex:([a-z0-9\w\s\d\-]+)?>', 'Controller@action');
        $this->assertEquals('(?:\/([a-z0-9\w\s\d\-]+))?\/?', $route->compile());
        $this->assertEquals(Map {'regex' => '([a-z0-9\w\s\d\-]+)'}, $route->getPatterns());

        $route = new Route('/{string}/[int]', 'Controller@action');
        $this->assertEquals('\/([a-z\_\-\+]+)\/([0-9\.]+)\/?', $route->compile());

        $route = new Route('/(wild)/{string}/[int?]', 'Controller@action');
        $this->assertEquals('\/(.*)\/([a-z\_\-\+]+)(?:\/([0-9\.]+))?\/?', $route->compile());

        $route = new Route('/(wild)/{string}/<regex:([a-z]\-[A-Z])>/[int?]', 'Controller@action');
        $this->assertEquals('\/(.*)\/([a-z\_\-\+]+)\/([a-z]\-[A-Z])(?:\/([0-9\.]+))?\/?', $route->compile());
    }

    public function testUrl() {
        $this->markTestIncomplete('@todo');
    }

}