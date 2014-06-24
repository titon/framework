<?php
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;

class XmlResponseTest extends TestCase {

    public function testSend() {
        $time = time();
        $response = new XmlResponse(['foo' => 'bar']);
        $response->prepare(Request::createFromGlobals());

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/xml; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Length' => [66],
        ], $response->getHeaders());

        $this->assertEquals('<?xml version="1.0" encoding="utf-8"?>' . "\n" .
            '<root><foo>bar</foo></root>', $body);
    }

    public function testRoot() {
        $time = time();
        $response = new XmlResponse(['foo' => 'bar'], 200, 'data');
        $response->prepare(Request::createFromGlobals());

        ob_start();
        $body = $response->send();
        ob_end_clean();

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/xml; charset=UTF-8'],
            'Status-Code' => ['200 OK'],
            'Content-Length' => [66],
        ], $response->getHeaders());

        $this->assertEquals('<?xml version="1.0" encoding="utf-8"?>' . "\n" .
            '<data><foo>bar</foo></data>', $body);
    }

}