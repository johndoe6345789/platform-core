#pragma once
/**
 * @file ContactController.h
 * @brief Contact form submission endpoint.
 *
 * Receives a contact enquiry and sends an email
 * notification to the admin address.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class ContactController
    : public drogon::HttpController<ContactController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ContactController::submit,
                  "/api/contact", drogon::Post);
    METHOD_LIST_END

    /**
     * @brief Handle contact form submission.
     * @param req Incoming HTTP request.
     * @param cb  Response callback.
     */
    void submit(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
