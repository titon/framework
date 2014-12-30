# HTTP #

Provides an object-oriented interface for the HTTP specification. Defines abstraction for the
request and response cycle, cookie management, and mimetype detection.
`Request` objects can be used to interact with GET, POST, FILES, SERVER, and COOKIE data.

```hack
$request = Titon\Http\Server\Request::createFromGlobals();
$request->isPost();
$request->post->get('foo');
```

`Response` objects can be used to modify HTTP headers, set the response body, status codes,
and to finally send output to the browser. Convenience methods for common HTTP headers are also available.

```hack
$response = Titon\Http\Server\Response::registry();
$response->body($output);
$response->statusCode(404);
$response->respond();
```