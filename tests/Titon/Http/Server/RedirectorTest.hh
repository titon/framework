<?hh // strict
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;
use Titon\Utility\State\Server;

class RedirectorTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        Redirector::$exit = false;
    }

    protected function tearDown(): void {
        parent::tearDown();

        Redirector::$exit = true;
    }

    public function testBack(): void {
        $_SERVER['HTTP_REFERER'] = '/new/url';
        Server::initialize($_SERVER);

        $time = time();
        $response = Redirector::back();
        $response->date($time);

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Location' => ['/new/url'],
            'Status-Code' => ['302 Found']
        ], $response->getHeaders());
    }

    public function testHome(): void {
        $time = time();
        $response = Redirector::home();
        $response->date($time);

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Location' => ['/'],
            'Status-Code' => ['302 Found']
        ], $response->getHeaders());
    }

    public function testRefresh(): void {
        $_SERVER['PATH_INFO'] = '/current/url';
        Server::initialize($_SERVER);

        $time = time();
        $response = Redirector::refresh();
        $response->date($time);

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Location' => ['/current/url'],
            'Status-Code' => ['302 Found']
        ], $response->getHeaders());
    }

    public function testTo(): void {
        $time = time();
        $response = Redirector::to('/custom/url', 300, function(Response $response): void {
            $response->setHeader('X-Test', 'Foobar');
        });
        $response->date($time);

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Location' => ['/custom/url'],
            'Status-Code' => ['300 Multiple Choices'],
            'X-Test' => ['Foobar']
        ], $response->getHeaders());
    }

}
