<?hh
namespace Titon\Route;

use Titon\Test\TestCase;

class RouteTest extends TestCase {

    public function testAppendPrepend() {
        $route = new Route('/', 'Controller@action');

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
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(Vector {}, $route->getFilters());

        $route->addFilter('foo');
        $route->addFilter('bar');
        $route->addFilter('foo');

        $this->assertEquals(Vector {'foo', 'bar'}, $route->getFilters());

        $route->setFilters(Vector {'baz'});

        $this->assertEquals(Vector {'baz'}, $route->getFilters());
    }

    public function testGetAction() {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals('Controller@action', $route->getAction());
    }

    public function testGetPath() {
        $route = new Route('/users/profile/[id]', 'Controller@action');

        $this->assertEquals('/users/profile/[id]', $route->getPath());
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
        $route = new Route('/{module}/{controller}/{action}.{ext}', 'Controller@action');

        $this->assertFalse($route->isMatch('/users'));
        $this->assertFalse($route->isMatch('/users/profile'));
        $this->assertFalse($route->isMatch('/users/profile/activity'));
        $this->assertTrue($route->isMatch('/users/profile/activity.json'));
    }

    public function testIsMatchAlphaPattern() {
        $route = new Route('/{alpha}', 'Controller@action');

        $this->assertTrue($route->isMatch('/FOO'));
        $this->assertTrue($route->isMatch('/foo'));
        $this->assertTrue($route->isMatch('/foo-bar'));
        $this->assertTrue($route->isMatch('/foo_bar'));
        $this->assertTrue($route->isMatch('/foo+bar'));
        $this->assertFalse($route->isMatch('/foo123'));
    }

    public function testIsMatchAlnumPattern() {
        $route = (new Route('/<alnum>', 'Controller@action'))->setPattern('alnum', Route::ALNUM);

        $this->assertTrue($route->isMatch('/FOO'));
        $this->assertTrue($route->isMatch('/foo'));
        $this->assertTrue($route->isMatch('/foo-bar'));
        $this->assertTrue($route->isMatch('/foo_bar'));
        $this->assertTrue($route->isMatch('/foo+bar'));
        $this->assertTrue($route->isMatch('/foo123'));
        $this->assertTrue($route->isMatch('/foo-123'));
        $this->assertFalse($route->isMatch('/foo.123'));
    }

    public function testIsMatchNumericPattern() {
        $route = new Route('/[numeric]', 'Controller@action');

        $this->assertTrue($route->isMatch('/123'));
        $this->assertTrue($route->isMatch('/12.3'));
        $this->assertFalse($route->isMatch('/12-3'));
        $this->assertFalse($route->isMatch('/123foo'));
        $this->assertFalse($route->isMatch('/foo'));
    }

    public function testIsMatchWildcardPattern() {
        $route = new Route('/(wild)', 'Controller@action');

        $this->assertTrue($route->isMatch('/FOO'));
        $this->assertTrue($route->isMatch('/foo'));
        $this->assertTrue($route->isMatch('/foo-bar'));
        $this->assertTrue($route->isMatch('/foo_bar'));
        $this->assertTrue($route->isMatch('/foo+bar'));
        $this->assertTrue($route->isMatch('/foo123'));
        $this->assertTrue($route->isMatch('/foo-123'));
        $this->assertTrue($route->isMatch('/foo.123'));
    }

    public function testIsMatchLocalePattern() {
        $route = (new Route('/<locale>', 'Controller@action'))->setPattern('locale', Route::LOCALE);

        $this->assertTrue($route->isMatch('/en'));
        $this->assertTrue($route->isMatch('/en-us'));
        $this->assertFalse($route->isMatch('/en_us'));
        $this->assertFalse($route->isMatch('/eng'));
    }

    public function testIsMatchMethod() {
        $route = new Route('/{module}', 'Controller@action');

        $_SERVER['REQUEST_METHOD'] = 'GET';

        $this->assertTrue($route->isMatch('/foo'));

        $route->addMethod('get');

        $this->assertTrue($route->isMatch('/foo'));

        $route->setMethods(Vector {'post'});

        $this->assertFalse($route->isMatch('/foo'));
    }

    public function testIsMatchSecure() {
        $route = new Route('/{module}', 'Controller@action');

        $_SERVER['HTTPS'] = 'off';

        $this->assertTrue($route->isMatch('/foo'));

        $route->setSecure(true);

        $this->assertFalse($route->isMatch('/foo'));

        $_SERVER['HTTPS'] = 'on';

        $this->assertTrue($route->isMatch('/foo'));
    }

    public function testIsMatchDirectEqualPath() {
        $route = new Route('/foo', 'Controller@action');

        $this->assertTrue($route->isMatch('/foo'));
    }

    public function testIsMatchTrailingSlash() {
        $route = new Route('/{module}', 'Controller@action');

        $this->assertTrue($route->isMatch('/users'));
        $this->assertTrue($route->isMatch('/users/'));
        $this->assertFalse($route->isMatch('/users//'));
    }

    public function testMethods() {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(Vector {}, $route->getMethods());

        $route->addMethod('get');
        $route->addMethod('post');
        $route->addMethod('get');

        $this->assertEquals(Vector {'get', 'post'}, $route->getMethods());

        $route->setMethods(Vector {'put'});

        $this->assertEquals(Vector {'put'}, $route->getMethods());
    }

    public function testParams() {
        $route = new Route('/{module}/{controller}/{action}.{ext}', 'Controller@action');
        $route->isMatch('/users/profile/activity.json');

        $this->assertEquals('/users/profile/activity.json', $route->url());

        $this->assertEquals(Map {
            'module' => 'users',
            'controller' => 'profile',
            'action' => 'activity',
            'ext' => 'json'
        }, $route->getParams());

        $this->assertEquals('users', $route->getParam('module'));
        $this->assertEquals(null, $route->getParam('hash'));
    }

    public function testPatterns() {
        $route = new Route('/', 'Controller@action');

        $this->assertEquals(Map {}, $route->getPatterns());

        $route->setPattern('id', '[0-9]+');
        $route->setPattern('key', '[a-z]+');
        $route->setPattern('id', '[1-8]+');

        $this->assertEquals(Map {'id' => '[1-8]+', 'key' => '[a-z]+'}, $route->getPatterns());

        $route->setPatterns(Map {'key' => '[A-Z]+'});

        $this->assertEquals(Map {'key' => '[A-Z]+'}, $route->getPatterns());
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
        $this->assertEquals('(?:\/([foo|bar]))?\/?', $route->compile());

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

}