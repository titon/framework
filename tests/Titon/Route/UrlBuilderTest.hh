<?hh
namespace Titon\Route;

use Titon\Context\Depository;
use Titon\Test\Stub\Route\TestRouteStub;
use Titon\Test\TestCase;
use Titon\Utility\Config;
use Titon\Utility\State\Get;
use Titon\Utility\State\Server;

/**
 * @property \Titon\Route\UrlBuilder $object
 */
class UrlBuilderTest extends TestCase {

    public static function setUpBeforeClass(): void {
        $container = Depository::getInstance();
        $container->singleton('Titon\Route\Router');
        $container->register('Titon\Route\UrlBuilder')->with($container->make('Titon\Route\Router'));
    }

    public static function tearDownAfterClass(): void {
        Depository::getInstance()->clear();
    }

    protected function setUp(): void {
        parent::setUp();

        $container = Depository::getInstance();

        $router = $container->make('Titon\Route\Router');

        invariant($router instanceof Router, 'Must be a Router.');

        $router->map('action.ext', new TestRouteStub('/{module}/{controller}/{action}.{ext}', 'Module\Controller@action'));
        $router->map('action', new TestRouteStub('/{module}/{controller}/{action}', 'Module\Controller@action'));
        $router->map('controller', new TestRouteStub('/{module}/{controller}', 'Module\Controller@action'));
        $router->map('module', new TestRouteStub('/{module}', 'Module\Controller@action'));
        $router->map('root', new TestRouteStub('/', 'Module\Controller@action'));

        $this->object = $builder = $container->make('Titon\Route\UrlBuilder');

        invariant($builder instanceof UrlBuilder, 'Must be a UrlBuilder.');
    }

    public function testBuild(): void {
        $this->assertEquals('/', $this->object->build('root'));
        $this->assertEquals('/users', $this->object->build('module', Map {'module' => 'users'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    public function testBuildQueryString(): void {
        $this->assertEquals('/users?foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar'}));
        $this->assertEquals('/users?foo=bar&baz%5B0%5D=1&baz%5B1%5D=2&baz%5B2%5D=3', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar', 'baz' => Vector{1, 2, 3}}));
    }

    public function testBuildHashFragment(): void {
        $this->assertEquals('/users#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar'}));
        $this->assertEquals('/users#foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => Map {'foo' => 'bar'}}));
        $this->assertEquals('/users?key=value#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar', 'key' => 'value'}));
    }

    public function testBuildOptionalToken(): void {
        $this->object->getRouter()->map('blog.archives', new TestRouteStub('/blog/[year]/[month]/[day?]', 'Module\Controller@action'));

        $this->assertEquals('/blog/2012/2', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02}));
        $this->assertEquals('/blog/2012/2/26', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02, 'day' => 26}));
    }

    public function testBuildInBaseFolder(): void {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/index.php';

        Server::initialize($_SERVER);

        $router = new Router();
        $router->map('module', new TestRouteStub('/{module}', 'Module\Controller@action'));

        $builder = new UrlBuilder($router);

        $this->assertEquals('/base', $builder->getBase());
        $this->assertEquals('/base/users', $builder->build('module', Map {'module' => 'users'}));
    }

    public function testBuildWithLocale(): void {
        $route = new Route('/<locale>/{module}', 'Module\Controller@action');
        $route->addPattern('locale', Route::LOCALE)->compile();

        $this->object->getRouter()->map('locale', $route);

        $this->assertEquals('/fr-fr/forum', $this->object->build('locale', Map {'module' => 'forum', 'locale' => 'fr-fr'}));

        Config::set('titon.locale.current', 'en_US');

        $this->assertEquals('/en-us/forum', $this->object->build('locale', Map {'module' => 'forum'}));

        Config::remove('titon.locale.current');
    }

    public function testBuildTokenInflection(): void {
        $this->assertEquals('/download-center', $this->object->build('module', Map {'module' => 'download_center'}));
        $this->assertEquals('/customer-support', $this->object->build('module', Map {'module' => 'cusToMer-SuPPorT@#&(#'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'Users', 'controller' => 'ProfILE', 'action' => 'feeD', 'ext' => 'JSON'}));
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingRouteException
     */
    public function testBuildInvalidKey(): void {
        $this->object->build('foobar');
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingTokenException
     */
    public function testBuildMissingToken(): void {
        $this->object->build('module');
    }

    public function testGetAbsoluteUrl(): void {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/app/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123?foo=bar';
        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $_GET = ['foo' => 'bar'];
        Get::initialize($_GET);

        $builder = new UrlBuilder(new Router());

        $this->assertEquals('https://sub.domain.com/base/app/module/controller/action.html/123?foo=bar', $builder->getAbsoluteUrl());
    }

    public function testSegments(): void {
        $router = new Router();

        $_SERVER['DOCUMENT_ROOT'] = '';
        $_SERVER['HTTP_HOST'] = 'localhost';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/';
        Server::initialize($_SERVER);

        $builder = new UrlBuilder($router);
        $this->assertEquals('/', $builder->getBase());
        $this->assertEquals('/', $builder->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $builder->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'localhost',
            'port' => 80
        }, $builder->getSegments());

        // module, controller
        $_SERVER['HTTP_HOST'] = 'domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/index.php';
        $_SERVER['REQUEST_URI'] = '/module/index';
        Server::initialize($_SERVER);

        $builder = new UrlBuilder($router);
        $this->assertEquals('/', $builder->getBase());
        $this->assertEquals('/module/index', $builder->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $builder->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/index',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'domain.com',
            'port' => 80
        }, $builder->getSegments());

        // module, controller, action, ext, base,
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/dir/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html';
        Server::initialize($_SERVER);

        $builder = new UrlBuilder($router);
        $this->assertEquals('/root/dir', $builder->getBase());
        $this->assertEquals('/module/controller/action.html', $builder->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $builder->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html',
            'scheme' => 'http',
            'query' => Map {},
            'host' => 'sub.domain.com',
            'port' => 80
        }, $builder->getSegments());

        // module, controller, action, ext, base, query, https
        $_SERVER['HTTP_HOST'] = 'subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $_GET = ['foo' => 'bar', 'int' => 123];
        Get::initialize($_GET);

        $builder = new UrlBuilder($router);
        $this->assertEquals('/rooter/root/dir', $builder->getBase());
        $this->assertEquals('/module/controller/action.html', $builder->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $builder->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html',
            'scheme' => 'https',
            'query' => Map {'foo' => 'bar', 'int' => 123},
            'host' => 'subber.sub.domain.com',
            'port' => 80
        }, $builder->getSegments());

        // module, controller, action, ext, base, query, https, args
        $_SERVER['HTTP_HOST'] = 'subbest.subber.sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/base/rooter/root/dir/index.php'; // query doesn't show up here
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123/abc?foo=bar&int=123';
        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $_GET = ['foo' => 'bar', 'int' => 123];
        Get::initialize($_GET);

        $builder = new UrlBuilder($router);
        $this->assertEquals('/base/rooter/root/dir', $builder->getBase());
        $this->assertEquals('/module/controller/action.html/123/abc', $builder->getSegment('path'));
        $this->assertInstanceOf('HH\Map', $builder->getSegment('query'));
        $this->assertEquals(Map {
            'path' => '/module/controller/action.html/123/abc',
            'scheme' => 'https',
            'query' => Map {'foo' => 'bar', 'int' => 123},
            'host' => 'subbest.subber.sub.domain.com',
            'port' => 80
        }, $builder->getSegments());
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingSegmentException
     */
    public function testSegmentsMissingKey(): void {
        $this->object->getSegment('fakeKey');
    }

    public function testUrlGlobalFunction(): void {
        $this->assertEquals('/users/profile/feed.json', url('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    public function testCurrentUrlGlobalFunction(): void {
        $this->assertEquals('http://localhost/', current_url());
    }

}
