<?hh
namespace Titon\Route;

use Titon\Utility\Config;
use Titon\Test\TestCase;

/**
 * @property \Titon\Route\Router $object
 */
class RouterTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = Router::registry();
        $this->object->map('action.ext', new Route('/{module}/{controller}/{action}.{ext}', 'Module\Controller@action'));
        $this->object->map('action', new Route('/{module}/{controller}/{action}', 'Module\Controller@action'));
        $this->object->map('controller', new Route('/{module}/{controller}', 'Module\Controller@action'));
        $this->object->map('module', new Route('/{module}', 'Module\Controller@action'));
        $this->object->map('root', new Route('/', 'Module\Controller@action'));
    }

    public function testBuild() {
        $this->assertEquals('/', $this->object->build('root'));
        $this->assertEquals('/users', $this->object->build('module', Map {'module' => 'users'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    public function testBuildQueryString() {
        $this->assertEquals('/users?foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar'}));
        $this->assertEquals('/users?foo=bar&baz%5B0%5D=1&baz%5B1%5D=2&baz%5B2%5D=3', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar', 'baz' => Vector{1, 2, 3}}));
    }

    public function testBuildHashFragment() {
        $this->assertEquals('/users#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar'}));
        $this->assertEquals('/users#foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => Map {'foo' => 'bar'}}));
        $this->assertEquals('/users?key=value#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar', 'key' => 'value'}));
    }

    public function testBuildOptionalToken() {
        $this->object->map('blog.archives', new Route('/blog/[year]/[month]/[day?]', 'Module\Controller@action'));

        $this->assertEquals('/blog/2012/2', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02}));
        $this->assertEquals('/blog/2012/2/26', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02, 'day' => 26}));
    }

    public function testBuildInBaseFolder() {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/index.php';

        $router = new Router();
        $router->map('module', new Route('/{module}', 'Module\Controller@action'));

        $this->assertEquals('/base', $router->base());
        $this->assertEquals('/base/users', $router->build('module', Map {'module' => 'users'}));
    }

    public function testBuildWithLocale() {
        $this->object->map('locale', (new Route('/<locale>/{module}', 'Module\Controller@action'))->addPattern('locale', Route::LOCALE));

        $this->assertEquals('/fr-fr/forum', $this->object->build('locale', Map {'module' => 'forum', 'locale' => 'fr-fr'}));

        Config::set('titon.locale.current', 'en_US');

        $this->assertEquals('/en-us/forum', $this->object->build('locale', Map {'module' => 'forum'}));

        Config::remove('titon.locale.current');
    }

    public function testBuildTokenInflection() {
        $this->assertEquals('/download-center', $this->object->build('module', Map {'module' => 'download_center'}));
        $this->assertEquals('/customer-support', $this->object->build('module', Map {'module' => 'cusToMer-SuPPorT@#&(#'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'Users', 'controller' => 'ProfILE', 'action' => 'feeD', 'ext' => 'JSON'}));
    }

    public function testBuildUrlFunction() {
        $this->assertEquals('/users/profile/feed.json', url('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingRouteException
     */
    public function testBuildInvalidKey() {
        $this->object->build('foobar');
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingTokenException
     */
    public function testBuildMissingToken() {
        $this->object->build('module');
    }

    public function testBuildAction() {
        $this->assertEquals('Controller@action', Router::buildAction(shape('class' => 'Controller', 'action' => 'action')));
    }

    public function testFilters() {
        $stub = new FilterStub();

        $this->object->filter('test', $stub);
        $this->object->filterCallback('test2', function() {});

        $this->assertEquals(inst_meth($stub, 'filter'), $this->object->getFilter('test'));
        $this->assertEquals(Vector {'test', 'test2'}, $this->object->getFilters()->keys());

        // Filtering is passed to routes
        $this->object->map('f1', (new Route('/f1', 'Controller@action'))->addFilter('test2'));
        $this->object->group(Map {'filters' => Vector {'test'}}, function(Router $router) {
            $router->map('f2', new Route('/f2', 'Controller@action'));
        });
        $this->object->map('f3', new Route('/f3', 'Controller@action'));

        $routes = $this->object->all();

        $this->assertEquals(Vector {'test2'}, $routes['f1']->getFilters());
        $this->assertEquals(Vector {'test'}, $routes['f2']->getFilters());
        $this->assertEquals(Vector {}, $routes['f3']->getFilters());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingFilterException
     */
    public function testFilterMissingKey() {
        $this->object->getFilter('fakeKey');
    }

    public function testFilterIsTriggered() {
        $router = new Router();
        $count = 0;

        $router->filterCallback('test', function() use (&$count) {
            $count++;
        });

        $router->map('f1', (new Route('/f1', 'Controller@action'))->addFilter('test'));
        $router->group(Map {'filters' => Vector {'test'}}, function(Router $router) {
            $router->map('f2', new Route('/f2', 'Controller@action'));
        });
        $router->map('f3', new Route('/f3', 'Controller@action'));

        $router->match('/f1');
        $this->assertEquals(1, $count);

        $router->match('/f2');
        $this->assertEquals(2, $count);

        $router->match('/f3');
        $this->assertEquals(2, $count);
    }

    /**
     * @expectedException \Exception
     */
    public function testFilterCanThrowException() {
        $this->object->filterCallback('test', function() use (&$count) {
            throw new \Exception('Filter error!');
        });

        $this->object->map('root', (new Route('/', 'Controller@action'))->addFilter('test'));

        $this->object->match('/');
    }

    public function testGroupPrefixing() {
        $this->object->group(Map {'prefix' => '/pre/'}, function(Router $router) {
            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', new Route('/group-2', 'Controller@action'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/pre/group-1', $routes['group1']->getPath());
        $this->assertEquals('/pre/group-2', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupSuffixing() {
        $this->object->group(Map {'suffix' => '/post/'}, function(Router $router) {
            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', new Route('/group-2', 'Controller@action'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/group-1/post', $routes['group1']->getPath());
        $this->assertEquals('/group-2/post', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupSecure() {
        $this->object->group(Map {'secure' => true}, function(Router $router) {
            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', new Route('/group-2', 'Controller@action'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->all();

        $this->assertEquals(false, $routes['root']->getSecure());
        $this->assertEquals(true, $routes['group1']->getSecure());
        $this->assertEquals(true, $routes['group2']->getSecure());
        $this->assertEquals(false, $routes['solo']->getSecure());
    }

    public function testGroupPatterns() {
        $this->object->group(Map {'prefix' => '<token>', 'patterns' => Map {'token' => '([abcd]+)'}}, function(Router $router) {
            $router->map('group1', new Route('/group-1', 'Controller@action'));
            $router->map('group2', (new Route('/group-2', 'Controller@action'))->addPattern('foo', '(bar|baz)'));
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/<token>/group-1', $routes['group1']->getPath());
        $this->assertEquals('/<token>/group-2', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());

        $this->assertEquals(Map {}, $routes['root']->getPatterns());
        $this->assertEquals(Map {'token' => '([abcd]+)'}, $routes['group1']->getPatterns());
        $this->assertEquals(Map {'foo' => '(bar|baz)', 'token' => '([abcd]+)'}, $routes['group2']->getPatterns());
        $this->assertEquals(Map {}, $routes['solo']->getPatterns());
    }

    public function testGroupNesting() {
        $this->object->group(Map {'prefix' => '/pre/'}, function(Router $router) {
            $router->map('group1', new Route('/group-1', 'Controller@action'));

            $router->group(Map {'suffix' => '/post'}, function(Router $router) {
                $router->map('group2', new Route('/group-2', 'Controller@action'));
            });
        });

        $this->object->map('solo', new Route('/solo', 'Controller@action'));

        $routes = $this->object->all();

        $this->assertEquals('/', $routes['root']->getPath());
        $this->assertEquals('/pre/group-1', $routes['group1']->getPath());
        $this->assertEquals('/pre/group-2/post', $routes['group2']->getPath());
        $this->assertEquals('/solo', $routes['solo']->getPath());
    }

    public function testGroupNestingInherits() {
        $this->markTestIncomplete();
    }

    public function testHttpMapping() {
        $this->object->map('url1', new Route('/url', 'Controller@action'));
        $this->object->get('url2', new Route('/url', 'Controller@action'));
        $this->object->post('url3', new Route('/url', 'Controller@action'));
        $this->object->put('url4', new Route('/url', 'Controller@action'));
        $this->object->delete('url5', new Route('/url', 'Controller@action'));

        $routes = $this->object->all();

        $this->assertEquals(Vector {}, $routes['url1']->getMethods());
        $this->assertEquals(Vector {'get', 'head'}, $routes['url2']->getMethods());
        $this->assertEquals(Vector {'post'}, $routes['url3']->getMethods());
        $this->assertEquals(Vector {'put'}, $routes['url4']->getMethods());
        $this->assertEquals(Vector {'delete'}, $routes['url5']->getMethods());
    }

    public function testLoopMatch() {
        $route = $this->object->match('/');
        $this->assertEquals('/', $route->getPath());
        $this->assertEquals($route, $this->object->current());

        $route = $this->object->match('/users');
        $this->assertEquals('/{module}', $route->getPath());

        $route = $this->object->match('/users/profile');
        $this->assertEquals('/{module}/{controller}', $route->getPath());

        $route = $this->object->match('/users/profile/view');
        $this->assertEquals('/{module}/{controller}/{action}', $route->getPath());

        $route = $this->object->match('/users/profile/view.json');
        $this->assertEquals('/{module}/{controller}/{action}.{ext}', $route->getPath());
    }

    /**
     * @expectedException \Titon\Route\Exception\NoMatchException
     */
    public function testLoopMatchNoMatch() {
        $this->object->match('/path~tilde');
    }

    public function testParseAction() {
        $this->assertEquals(shape(
            'class' => 'Controller',
            'action' => 'action'
        ), Router::parseAction('Controller@action'));

        $this->assertEquals(shape(
            'class' => 'Controller',
            'action' => 'foo'
        ), Router::parseAction('Controller@foo'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'index'
        ), Router::parseAction('Module\Controller@index'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller_With_Underscores',
            'action' => 'index'
        ), Router::parseAction('Module\Controller_With_Underscores@index'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller_With_Numbers123',
            'action' => 'index'
        ), Router::parseAction('Module\Controller_With_Numbers123@index'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'action'
        ), Router::parseAction('Module\Controller@action'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'multiWordAction'
        ), Router::parseAction('Module\Controller@multiWordAction'));

        $this->assertEquals(shape(
            'class' => 'Module\Controller',
            'action' => 'action_with_underscores'
        ), Router::parseAction('Module\Controller@action_with_underscores'));
    }

    /**
     * @expectedException \Titon\Route\Exception\InvalidRouteException
     */
    public function testParseActionInvalidRoute() {
        Router::parseAction('Broken+Route');
    }

    public function testResourceMap() {
        $this->assertEquals(Map {
            'list' => 'index',
            'create' => 'create',
            'read' => 'read',
            'update' => 'update',
            'delete' => 'delete'
        }, $this->object->getResourceMap());

        $this->object->setResourceMap(Map {
            'create' => 'add',
            'read' => 'view',
            'update' => 'edit',
            'delete' => 'remove'
        });

        $this->assertEquals(Map {
            'list' => 'index',
            'create' => 'add',
            'read' => 'view',
            'update' => 'edit',
            'delete' => 'remove'
        }, $this->object->getResourceMap());
    }

    public function testResourceMapping() {
        $this->object->resource('rest', new Route('/rest', 'Api\Rest@action'));

        $routes = $this->object->all();

        // Keys
        $this->assertFalse(isset($routes['rest']));
        $this->assertTrue(isset($routes['rest.list']));
        $this->assertTrue(isset($routes['rest.create']));
        $this->assertTrue(isset($routes['rest.read']));
        $this->assertTrue(isset($routes['rest.update']));
        $this->assertTrue(isset($routes['rest.delete']));

        // Paths
        $this->assertEquals('/rest', $routes['rest.list']->getPath());
        $this->assertEquals('/rest', $routes['rest.create']->getPath());
        $this->assertEquals('/rest/(id)', $routes['rest.read']->getPath());
        $this->assertEquals('/rest/(id)', $routes['rest.update']->getPath());
        $this->assertEquals('/rest/(id)', $routes['rest.delete']->getPath());

        // Action
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'index'), $routes['rest.list']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'create'), $routes['rest.create']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'read'), $routes['rest.read']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'update'), $routes['rest.update']->getAction());
        $this->assertEquals(shape('class' => 'Api\Rest', 'action' => 'delete'), $routes['rest.delete']->getAction());

        // Method
        $this->assertEquals(Vector {'get'}, $routes['rest.list']->getMethods());
        $this->assertEquals(Vector {'post'}, $routes['rest.create']->getMethods());
        $this->assertEquals(Vector {'get'}, $routes['rest.read']->getMethods());
        $this->assertEquals(Vector {'put', 'post'}, $routes['rest.update']->getMethods());
        $this->assertEquals(Vector {'delete', 'post'}, $routes['rest.delete']->getMethods());
    }

    public function testRoutes() {
        $route = new Route('/', 'Controller@action');
        $router = new Router();
        $router->map('key', $route);

        $this->assertEquals($route, $router->getRoute('key'));
        $this->assertEquals(Map {'key' => $route}, $router->getRoutes());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingRouteException
     */
    public function testRoutesMissingKey() {
        $this->object->getRoute('fakeKey');
    }

    public function testSegments() {
        $_SERVER['DOCUMENT_ROOT'] = '';
        $_SERVER['HTTP_HOST'] = 'localhost';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/';

        $router = new Router();
        $this->assertEquals('/', $router->base());
        $this->assertEquals('http://localhost/', $router->url());
        $this->assertEquals('/', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'localhost',
            'port' => 80
        }, $router->getSegments());

        // module, controller
        $_SERVER['HTTP_HOST'] = 'domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/module/index';

        $router = new Router();
        $this->assertEquals('/', $router->base());
        $this->assertEquals('http://domain.com/module/index', $router->url());
        $this->assertEquals('/module/index', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/index',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'domain.com',
            'port' => 80
        }, $router->getSegments());

        // module, controller, action, ext, base,
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/dir/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html';

        $router = new Router();
        $this->assertEquals('/root/dir', $router->base());
        $this->assertEquals('http://sub.domain.com/root/dir/module/controller/action.html', $router->url());
        $this->assertEquals('/module/controller/action.html', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'sub.domain.com',
            'port' => 80
        }, $router->getSegments());

        // module, controller, action, ext, base, query, https
        $_SERVER['HTTP_HOST'] = 'subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        $_GET = ['foo' => 'bar', 'int' => 123];

        $router = new Router();
        $this->assertEquals('/rooter/root/dir', $router->base());
        $this->assertEquals('https://subber.sub.domain.com/rooter/root/dir/module/controller/action.html?foo=bar&int=123', $router->url());
        $this->assertEquals('/module/controller/action.html', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html',
            'scheme' => 'https',
            'query' => Map {'foo' => 'bar', 'int' => 123},
            'host' => 'subber.sub.domain.com',
            'port' => 80
        }, $router->getSegments());

        // module, controller, action, ext, base, query, https, args
        $_SERVER['HTTP_HOST'] = 'subbest.subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/base/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123/abc?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        $_GET = ['foo' => 'bar', 'int' => 123];

        $router = new Router();
        $this->assertEquals('/base/rooter/root/dir', $router->base());
        $this->assertEquals('https://subbest.subber.sub.domain.com/base/rooter/root/dir/module/controller/action.html/123/abc?foo=bar&int=123', $router->url());
        $this->assertEquals('/module/controller/action.html/123/abc', $router->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $router->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html/123/abc',
            'scheme' => 'https',
            'query' => Map {'foo' => 'bar', 'int' => 123},
            'host' => 'subbest.subber.sub.domain.com',
            'port' => 80
        }, $router->getSegments());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingSegmentException
     */
    public function testSegmentsMissingKey() {
        $this->object->getSegment('fakeKey');
    }

    public function testUrl() {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/app/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123?foo=bar';
        $_SERVER['HTTPS'] = 'on';
        $_GET = ['foo' => 'bar'];

        $router = new Router();

        $this->assertEquals('https://sub.domain.com/base/app/module/controller/action.html/123?foo=bar', $router->url());
    }

}

class FilterStub implements Filter {

    public function filter(Router $router, Route $route): void {
        return;
    }

}