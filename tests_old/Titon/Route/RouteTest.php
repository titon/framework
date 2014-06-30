<?php
namespace Titon\Route;

use Titon\Test\TestCase;

class RouteTest extends TestCase {

    public function testTokens() {
        $route = new Route('/{string}');
        $this->assertEquals('\/([a-z\_\-\+]+)\/?', $route->compile());

        $route = new Route('/{string?}');
        $this->assertEquals('(?:\/([a-z\_\-\+]+))?\/?', $route->compile());

        $route = new Route('/[int]');
        $this->assertEquals('\/([0-9\.]+)\/?', $route->compile());

        $route = new Route('/[int?]');
        $this->assertEquals('(?:\/([0-9\.]+))?\/?', $route->compile());

        $route = new Route('/(wildcard)');
        $this->assertEquals('\/(.*)\/?', $route->compile());

        $route = new Route('/(wildcard?)');
        $this->assertEquals('(?:\/(.*))?\/?', $route->compile());

        $route = new Route('/<regex>', [], ['patterns' => ['regex' => '[foo|bar]']]);
        $this->assertEquals('\/([foo|bar])\/?', $route->compile());

        $route = new Route('/<regex?>', [], ['patterns' => ['regex' => '[foo|bar]']]);
        $this->assertEquals('(?:\/([foo|bar]))?\/?', $route->compile());

        $route = new Route('/<regex:[foo|bar]>');
        $this->assertEquals('\/([foo|bar])\/?', $route->compile());

        $route = new Route('/<regex:[foo|bar]?>');
        $this->assertEquals('(?:\/([foo|bar]))?\/?', $route->compile());

        $route = new Route('/<regex:([a-z0-9\w\s\d\-]+)?>');
        $this->assertEquals('(?:\/([a-z0-9\w\s\d\-]+))?\/?', $route->compile());
        $this->assertEquals(['regex' => '([a-z0-9\w\s\d\-]+)'], $route->getPatterns());

        $route = new Route('/{string}/[int]');
        $this->assertEquals('\/([a-z\_\-\+]+)\/([0-9\.]+)\/?', $route->compile());

        $route = new Route('/(wild)/{string}/[int?]');
        $this->assertEquals('\/(.*)\/([a-z\_\-\+]+)(?:\/([0-9\.]+))?\/?', $route->compile());

        $route = new Route('/(wild)/{string}/<regex:([a-z]\-[A-Z])>/[int?]');
        $this->assertEquals('\/(.*)\/([a-z\_\-\+]+)\/([a-z]\-[A-Z])(?:\/([0-9\.]+))?\/?', $route->compile());
    }

    public function testCompile() {
        $moduleControllerActionExt = new Route('/{module}/{controller}/{action}.{ext}');
        $moduleControllerAction = new Route('/{module}/{controller}/{action}');
        $moduleController = new Route('/{module}/{controller}');
        $module = new Route('/{module}');
        $root = new Route('/', [], ['static' => true]);

        $this->assertEquals('\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\.([a-z\_\-\+]+)\/?', $moduleControllerActionExt->compile());
        $this->assertEquals('\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/?', $moduleControllerAction->compile());
        $this->assertEquals('\/([a-z\_\-\+]+)\/([a-z\_\-\+]+)\/?', $moduleController->compile());
        $this->assertEquals('\/([a-z\_\-\+]+)\/?', $module->compile());
        $this->assertEquals('\/', $root->compile());

        $multi = new Route('{alpha}/[numeric]/(wildcard)/');

        $patterns = new Route('<alnum>/<locale>', [], [
            'patterns' => [
                'alnum' => Route::ALNUM,
                'locale' => '([a-z]{2}(?:-[a-z]{2})?)'
            ]
        ]);

        $allTypes = new Route('/<locale>/{alpha}/(wildcard)/[numeric]/{alnum}', [], [
            'patterns' => [
                'alnum' => Route::ALNUM,
                'locale' => '([a-z]{2}(?:-[a-z]{2})?)'
            ]
        ]);

        $this->assertEquals('\/([a-z\_\-\+]+)\/([0-9\.]+)\/(.*)\/?', $multi->compile());
        $this->assertEquals('\/([a-z0-9\_\-\+]+)\/([a-z]{2}(?:-[a-z]{2})?)\/?', $patterns->compile());
        $this->assertEquals('\/([a-z]{2}(?:-[a-z]{2})?)\/([a-z\_\-\+]+)\/(.*)\/([0-9\.]+)\/([a-z\_\-\+]+)\/?', $allTypes->compile());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingPatternException
     */
    public function testCompileMissingPattern() {
        $noPattern = new Route('<missing>');
        $noPattern->compile();
    }

    public function testCompileOptionalToken() {
        $route = new Route('/users/[id]');

        $this->assertEquals('\/users\/([0-9\.]+)\/?', $route->compile());
        $this->assertTrue($route->isMatch('/users/1'));
        $this->assertFalse($route->isMatch('/users'));

        $route = new Route('/users/[id?]');

        $this->assertEquals('\/users(?:\/([0-9\.]+))?\/?', $route->compile());
        $this->assertTrue($route->isMatch('/users/1'));
        $this->assertTrue($route->isMatch('/users/'));
        $this->assertTrue($route->isMatch('/users'));
    }

    public function testIsMatchModuleControllerActionExt() {
        $url = '/forum/topic/stats.html';
        $route = new Route('/{module}/{controller}/{action}.{ext}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => 'html',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'stats',
            'query' => [],
            'args' => []
        ], $route->getParams());

        // single argument
        $url = '/forum/topic/view/123.xml';
        $route = new Route('/{module}/{controller}/{action}/[id].{ext}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => 'xml',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'view',
            'query' => [],
            'args' => [],
            'id' => 123
        ], $route->getParams());

        // multi arguments
        $url = '/forum/topic/view/123/abc.json';
        $route = new Route('/{module}/{controller}/{action}/[id]/{slug}.{ext}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => 'json',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'view',
            'query' => [],
            'args' => [],
            'id' => 123,
            'slug' => 'abc'
        ], $route->getParams());

        // invalid module
        $url = '/foobar/dashboard/edit.xhtml';
        $route = new Route('/{module}/{controller}/{action}.{ext}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => 'xhtml',
            'module' => 'foobar',
            'controller' => 'dashboard',
            'action' => 'edit',
            'query' => [],
            'args' => []
        ], $route->getParams());

        // invalid controller
        $url = '/forum/foobar/view/123.php';
        $route = new Route('/{module}/{controller}/{action}/[id].{ext}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => 'php',
            'module' => 'forum',
            'controller' => 'foobar',
            'action' => 'view',
            'query' => [],
            'args' => [],
            'id' => 123
        ], $route->getParams());
    }

    public function testIsMatchModuleControllerAction() {
        $url = '/forum/topic/stats';
        $route = new Route('/{module}/{controller}/{action}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'stats',
            'query' => [],
            'args' => []
        ], $route->getParams());

        // single argument
        $url = '/forum/topic/view/123';
        $route = new Route('/{module}/{controller}/{action}/[id]');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'view',
            'query' => [],
            'args' => [],
            'id' => 123
        ], $route->getParams());

        // multi arguments
        $url = '/forum/topic/view/123/abc';
        $route = new Route('/{module}/{controller}/{action}/[id]/{slug}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'view',
            'query' => [],
            'args' => [],
            'id' => 123,
            'slug' => 'abc'
        ], $route->getParams());

        // invalid module
        $url = '/foobar/dashboard/edit';
        $route = new Route('/{module}/{controller}/{action}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'foobar',
            'controller' => 'dashboard',
            'action' => 'edit',
            'query' => [],
            'args' => []
        ], $route->getParams());

        // invalid controller
        $url = '/forum/foobar/view/123';
        $route = new Route('/{module}/{controller}/{action}/[id]');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'forum',
            'controller' => 'foobar',
            'action' => 'view',
            'query' => [],
            'args' => [],
            'id' => 123
        ], $route->getParams());
    }

    public function testIsMatchModuleController() {
        $url = '/forum/topic';
        $route = new Route('/{module}/{controller}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'forum',
            'controller' => 'topic',
            'action' => 'index',
            'query' => [],
            'args' => []
        ], $route->getParams());

        // invalid controller
        $url = '/forum/foobar';
        $route = new Route('/{module}/{controller}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'forum',
            'controller' => 'foobar',
            'action' => 'index',
            'query' => [],
            'args' => []
        ], $route->getParams());
    }

    public function testIsMatchModule() {
        $url = '/users';
        $route = new Route('/{module}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'users',
            'controller' => 'index',
            'action' => 'index',
            'query' => [],
            'args' => []
        ], $route->getParams());

        // invalid controller
        $url = '/foobar';
        $route = new Route('/{module}');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'foobar',
            'controller' => 'index',
            'action' => 'index',
            'query' => [],
            'args' => []
        ], $route->getParams());
    }

    public function testIsMatch() {
        $url = '/';
        $route = new Route('/');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'main',
            'controller' => 'index',
            'action' => 'index',
            'query' => [],
            'args' => []
        ], $route->getParams());

        // Wrong method
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $route = new Route('/', [], ['method' => ['GET']]);

        $this->assertFalse($route->isMatch($url));

        // Not secure
        $_SERVER['HTTPS'] = 'off';
        $route = new Route('/', [], ['method' => ['POST'], 'secure' => true]);

        $this->assertFalse($route->isMatch($url));

        $_SERVER['HTTPS'] = 'on';
        $this->assertTrue($route->isMatch($url));
    }

    public function testMatchTrailingSlash() {
        $route = new Route('/{module}');

        $this->assertTrue($route->isMatch('/users'));
        $this->assertTrue($route->isMatch('/users/'));
        $this->assertFalse($route->isMatch('/users//'));
    }

    public function testIsMatchCustom() {
        $url = '/user/miles';
        $route = new Route('/user/{username}', [
            'module' => 'users',
            'controller' => 'profile'
        ]);

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'users',
            'controller' => 'profile',
            'action' => 'index',
            'query' => [],
            'args' => [],
            'username' => 'miles'
        ], $route->getParams());

        $url = '/blog/123456';
        $route = new Route('/blog/[id]', [
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'read'
        ]);

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'read',
            'query' => [],
            'args' => [],
            'id' => 123456
        ], $route->getParams());

        $url = '/blog/2012/02/26';
        $route = new Route('/blog/[year]/[month]/[day]', [
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'archives',
            'custom' => 'value'
        ]);

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'archives',
            'query' => [],
            'args' => [],
            'year' => 2012,
            'month' => 2,
            'day' => 26,
            'custom' => 'value'
        ], $route->getParams());

        $url = '/regex/123-abc';
        $route = new Route('/regex/<pattern>', [
            'module' => 'regex'
        ], [
            'patterns' => [
                'pattern' => '([0-9]+\-[a-z]+)'
            ]
        ]);

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'regex',
            'controller' => 'index',
            'action' => 'index',
            'query' => [],
            'args' => [],
            'pattern' => '123-abc'
        ], $route->getParams());

        // with @ format
        $url = '/blog/2012/02/26';
        $route = new Route('/blog/[year]/[month]/[day]', 'Blog\Api@archives');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'archives',
            'query' => [],
            'args' => [],
            'year' => 2012,
            'month' => 2,
            'day' => 26
        ], $route->getParams());

        $url = '/blog/123456';
        $route = new Route('/blog/[id]', 'Blog\Api@read');

        $this->assertTrue($route->isMatch($url));
        $this->assertEquals([
            'ext' => '',
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'read',
            'query' => [],
            'args' => [],
            'id' => 123456
        ], $route->getParams());
    }

    public function testIsMethod() {
        $noMethod = new Route('/');

        $singleMethod = new Route('/', [], ['method' => 'POST']);

        $multiMethod = new Route('/', [], ['method' => ['post', 'put']]);

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
        $unsecureRoute = new Route('/', [], ['secure' => false]);
        $secureRoute = new Route('/', [], ['secure' => true]);

        $this->assertTrue($unsecureRoute->isSecure());
        $this->assertFalse($secureRoute->isSecure());

        $_SERVER['HTTPS'] = 'on';

        $this->assertTrue($unsecureRoute->isSecure());
        $this->assertTrue($secureRoute->isSecure());
    }

    public function testIsStatic() {
        $route = new Route('/', [], ['static' => false]);
        $staticRoute = new Route('/', [], ['static' => true]);
        $tokenRoute = new Route('/{module}');

        $this->assertFalse($route->isStatic());
        $this->assertTrue($staticRoute->isStatic());
        $this->assertFalse($tokenRoute->isStatic());
    }

    public function testParamAndUrl() {
        $url = '/blog/2012/02/26';
        $route = new Route('/blog/[year]/[month]/[day]', [
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'archives',
            'custom' => 'value'
        ]);

        $route->isMatch($url);

        $this->assertEquals([
            'ext' => '',
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'archives',
            'query' => [],
            'args' => [],
            'year' => 2012,
            'month' => 2,
            'day' => 26,
            'custom' => 'value'
        ], $route->getParams());

        $this->assertEquals('blog', $route->getParam('module'));
        $this->assertEquals('archives', $route->getParam('action'));
        $this->assertEquals(2012, $route->getParam('year'));
        $this->assertEquals(2, $route->getParam('month'));
        $this->assertEquals(26, $route->getParam('day'));
        $this->assertEquals('value', $route->getParam('custom'));
        $this->assertEquals(null, $route->getParam('foobar'));
        $this->assertEquals($url, $route->url());
    }

    public function testArgPassing() {
        $url = '/blog/2012/02/26';
        $route = new Route('/blog/[year]/[month]/[day]', 'Blog\Api@archives', ['pass' => ['year', 'month', 'day']]);

        $route->isMatch($url);

        $this->assertEquals([
            'ext' => '',
            'module' => 'blog',
            'controller' => 'api',
            'action' => 'archives',
            'query' => [],
            'args' => [2012, 2, 26],
            'year' => 2012,
            'month' => 2,
            'day' => 26
        ], $route->getParams());
    }

    public function testAppendPrepend() {
        $route = new Route('');

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

}