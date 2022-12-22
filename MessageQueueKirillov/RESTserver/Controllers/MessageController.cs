using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

using RESTserver.Services;

namespace RESTserver.Controllers
{
    [Route("api/Broker")]
    [ApiController]
    public class MessageController : ControllerBase
    {
        // GET: api/<MessageController>
        [HttpGet]
        public IEnumerable<string> Get()
        {
            return new string[] { "value1", "value2" };
        }

        // GET api/<MessageController>/5
        [HttpGet("{id}")]
        public string Get(int id)
        {
            return "value";
        }

        // POST api/Broker/registration
        [HttpPost("registration")]
        public bool Registrate(string username)
        {
            if (MessageService.SendRegistrationRequest(username))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        // POST api/<MessageController>
        [HttpPost]
        public void Post([FromBody] string value)
        {
        }

        // PUT api/<MessageController>/5
        [HttpPut("{id}")]
        public void Put(int id, [FromBody] string value)
        {
        }

        // DELETE api/<MessageController>/5
        [HttpDelete("{id}")]
        public void Delete(int id)
        {
        }
    }
}
